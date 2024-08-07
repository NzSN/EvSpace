/* Structures defined here are isomorphic to some structures defined in C++ realm.
 * Particularly, you can't create structures defined here without information
 * from C++ realm. The purposes of these structures is to exchange
 * information from C++ realm */
import { load, Type as MessageType } from "protobufjs";
import { protoPath } from "./message";
import { waitFor } from "../base/utility";
import { assert } from "../base/assert";

/* Metadata return from C++ Realm */
export interface RingPipeMeta {
    message_type : string;
    pipe         : Uint8Array;
    rw_head      : Uint32Array;
    length       : number;

    notify_one   : () => void;
    notify_all   : () => void;
}
export interface BiPipeMeta {
    in  : RingPipeMeta;
    out : RingPipeMeta;
}

interface ReadMeta {
    r_idx: number;
    buffer_idx: number;
    buffer: Uint8Array,
};


export class RingPipe {
    private _message      : MessageType;
    private _pipe_buffer  : Uint8Array;
    private _rw_head      : Uint32Array;
    private _length       : number;
    private _begin        : number;
    private _end          : number;

    private _meta : RingPipeMeta;

    constructor(meta: RingPipeMeta) {

        this._meta = meta;
        assert(() => { return this._meta.length > 1; });

        this._pipe_buffer = this._meta.pipe;
        this._rw_head = this._meta.rw_head;
        this._length = this._meta.length;
        this._begin = 0;
        this._end = this._meta.length;

        assert(() => {
            return this._validityOfIdx(this._readIdx()) &&
                this._validityOfIdx(this._writeIdx());
        });
    }

    private async _declareMessage(msgType: string): Promise<MessageType> {
        const root = await load(protoPath(msgType) ?? "");
        return root.lookupType(msgType);
    }

    public notifyOne(): void {
        this._meta.notify_one();
    }

    public notifyAll(): void {
        this._meta.notify_all();
    }

    public async init(): Promise<void> {
        this._message = await this._declareMessage(this._meta.message_type);
    }

    public get length(): number {
        return this._length;
    }

    private _readIdx(): number {
        return this._rw_head[0];
    }

    private _setReadIdx(idx: number): void {
        this._rw_head[0] = idx;
    }

    private _updateReadIdx(idx: number): void {
        this._setReadIdx(idx);
        this.notifyOne();
    }

    private _writeIdx(): number {
        return this._rw_head[1];
    }

    private _setWriteIdx(idx: number): void {
        this._rw_head[1] = idx;
    }

    private _updateWriteIdx(idx: number): void {
        this._setWriteIdx(idx);
        this.notifyOne();
    }

    private _forwardRIDX(offset: number): void {
        this._updateReadIdx(this._nextN(this._readIdx(), offset));
    }

    private _forwardWIDX(offset: number): void {
        this._updateWriteIdx(this._nextN(this._writeIdx(), offset));
    }

    public isEmpty(): boolean {
        return this._readIdx() === this._writeIdx();
    }

    public isFull(): boolean {
        return this._length === 0 ||
            this._next(this._writeIdx()) === this._readIdx();
    }

    private _next(current: number): number {
        const next_pos = current + 1;
        if (next_pos >= this._end) {
            return 0;
        } else {
            return next_pos;
        }
    }

    private _nextN(current: number, steps: number): number {
        assert(() => { return this._validityOfIdx(current); },
               "_nextN failed: invalid idx");

        if (this._end - current - 1 >= steps) {
            return current + steps;
        } else {
            return this._begin + (steps - (this._end - current - 1)) - 1;
        }
    }

    private async _waitNonfull(): Promise<void> {
        // TODO: Implement this method in real blocked instead
        // of recursively checking condition by SetTimeout.
        // Currently, unable to calling JavaScript atomic from C++
        // because of pipe implement in node native module
        // does not use SharedArrayBuffer. ArrayBuffer is not
        // compatible with atomic in generic.
        //
        // Reference:
        // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise#browser_compatibility
        if (this.isFull()) {
            await waitFor(() => {
                return !this.isFull();
            });
        }
    }

    private async _waitNonempty(): Promise<void> {
        // TODO: Same as waitNonfull()
        if (this.isEmpty()) {
            await waitFor(() => {
                return !this.isEmpty();
            });
        }
    }

    // This method may be blocked.
    public async read(): Promise<any | null> {
        assert(() => { return this._validityOfIdx(this._readIdx()); },
               "Invalid idx");

        await this._waitNonempty();

        // Reading length field of message
        const rIdx = this._readIdx();
        const bytes_to_read = this._pipe_buffer[rIdx];
        assert(() => { return bytes_to_read > 0; });

        this._forwardRIDX(1);

        return await this._readFromBuffer(bytes_to_read);
    }

    private async _readFromBuffer(bytes_to_read: number): Promise<any | null> {
        const msg_buffer = new Uint8Array(bytes_to_read);

        let meta: ReadMeta = {
            r_idx: this._readIdx(),
            buffer_idx: 0,
            buffer: msg_buffer,
        };

        let remain = bytes_to_read;
        while (remain > 0) {
            let  bytes_in_buffer = Math.min(
                this._unreadBytesInBuffer(), remain);
            if (bytes_in_buffer == 0) {
                await this._waitNonempty();
                continue;
            }

            if (this._unreadBytesToEnd(meta.r_idx) > bytes_in_buffer) {
                const r_idx = meta.r_idx;
                const buffer_idx = meta.buffer_idx;
                const buffer_sub = this._pipe_buffer.subarray(r_idx, r_idx + bytes_in_buffer);
                msg_buffer.set(
                    buffer_sub,
                    buffer_idx);
                meta.r_idx = this._nextN(r_idx, bytes_in_buffer);
                meta.buffer_idx += bytes_in_buffer;
            } else {
                this._readCrossEnd(meta, bytes_in_buffer);
            }

            this._forwardRIDX(bytes_in_buffer);
            remain -= bytes_in_buffer;
        }

        assert(() => {
            return this._message.verify(msg_buffer) == null;
        }, "Invalid buffer");

        return this._message.decode(msg_buffer);
    }

    private async _readCrossEnd(meta: ReadMeta, bytes_in_buffer: number) {
        const r_idx: number = meta.r_idx;
        const buffer_idx: number = meta.buffer_idx;
        const buffer: Uint8Array = meta.buffer;

        const bytes_to_read_1 = this._end - r_idx;
        const bytes_to_read_2 = bytes_in_buffer - bytes_to_read_1;

        let bufToRead = this._pipe_buffer.subarray(
            r_idx, r_idx + bytes_to_read_1);
        buffer.set(bufToRead, buffer_idx);

        bufToRead = this._pipe_buffer.subarray(this._begin, this._begin + bytes_to_read_2);
        buffer.set(bufToRead, buffer_idx + bytes_to_read_1);

        meta.r_idx = this._nextN(r_idx, bytes_in_buffer);
        meta.buffer_idx = buffer_idx + bytes_in_buffer;
    }

    public async write(obj: object): Promise<boolean> {
        await this._waitNonfull();

        const buf = this._encoding(obj);
        if (buf === null) {
            return false;
        }

        const size_of_buf = buf.byteLength;
        assert(() => {
            // Curently size of size field is one byte
            return 0 <= size_of_buf && size_of_buf < Math.pow(2, 8);
        });
        // Write size field
        this._byteWrite(size_of_buf);
        this._forwardWIDX(1);
        // Write message datas
        return this._writeMessageIntoBuffer(buf);
    }

    private async _writeMessageIntoBuffer(buf: Uint8Array): Promise<boolean> {
        const size_of_buf = buf.byteLength;
        let buf_idx = 0;

        let remain = size_of_buf;
        while (remain > 0) {
            assert(() => { return buf_idx < size_of_buf; });

            let bytes_to_write = Math.min(
                this._freeBytesInBuffer(), remain);
            if (bytes_to_write === 0) {
                await this._waitNonfull();
                continue;
            }

            if (this._freeBytesToEnd(this._writeIdx()) > bytes_to_write) {
                /* Copy one time */
                this._pipe_buffer.set(
                    buf.subarray(buf_idx, bytes_to_write),
                    this._writeIdx());
            } else {
                /* Two times copy */
                const bytes_to_write_1 = this._end - this._writeIdx();
                this._pipe_buffer.set(
                    buf.subarray(buf_idx, buf_idx + bytes_to_write_1),
                    this._writeIdx());
                this._pipe_buffer.set(
                    buf.subarray(buf_idx + bytes_to_write_1,
                                 buf_idx + bytes_to_write),
                    this._begin);
            }

            this._forwardWIDX(bytes_to_write);
            buf_idx += bytes_to_write;
            remain -= bytes_to_write;
        }
        return true;
    }

    private _encoding(obj: Object): Uint8Array | null {
        if (this._message.verify(obj)) {
            return null;
        } else {
            return this._message.encode(obj).finish();
        }
    }

    private _byteWrite(byte: number) {
        assert(() => { return 0 <= byte && 0 < Math.pow(2,8); });
        this._pipe_buffer[this._writeIdx()] = byte;
    }

    private _byteRead(): number {
        return this._pipe_buffer[this._readIdx()];
    }

    private _validityOfIdx(idx: number): boolean {
        return this._begin <= idx && idx <= this._end;
    }

    private _unreadBytesToEnd(idx: number): number {
        return this._end - idx;
    }

    private _unreadBytesInBuffer(): number {
        const r_idx = this._readIdx(), w_idx = this._writeIdx();

        if (r_idx > w_idx) {
            return this._length - (r_idx - w_idx);
        } else {
            return w_idx - r_idx;
        }
    }

    private _freeBytesInBuffer(): number {
        return this._length - this._unreadBytesInBuffer() - 1;
    }

    private _freeBytesToEnd(idx: number) {
        return this._end - idx - 1;
    }
}

export class BiPipe {
    private _in   : RingPipe;
    private _out  : RingPipe;
    private _meta : BiPipeMeta;

    constructor(meta: BiPipeMeta) {
        this._meta = meta;
        this._in = new RingPipe(this._meta.in);
        this._out = new RingPipe(this._meta.out);
    }

    async init() {
        await this._in.init();
        await this._out.init();
    }

    public async read(): Promise<any | null> {
        return await this._in.read();
    }

    public async write(message: object): Promise<boolean> {
        return await this._out.write(message);
    }

    public readable(): boolean {
        return !this._in.isEmpty();
    }

    public writable(): boolean {
        return !this._out.isFull();
    }
}
