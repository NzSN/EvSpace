/* Structures defined here are isomorphic to some structures defined in C++ realm.
 * Particularly, you can't create structures defined here without information
 * from C++ realm. The purposes of these structures is to exchange
 * information from C++ realm */
import { load, Type as MessageType, Message } from "protobufjs";
import { protoPath } from "./message";
import { waitFor } from "../base/utility";
import { assert } from "../base/assert";

/* Metadata return from C++ Realm */
export interface RingPipeMeta {
    message_type : string;
    pipe         : Uint8Array;
    rw_head      : Uint32Array;
    length       : number;

    notifyOne   : () => void;
    notifyAll   : () => void;
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

    private _notifyOne   : () => void;
    private _notifyAll   : () => void;

    private _meta : RingPipeMeta;

    constructor(meta: RingPipeMeta) {
        assert(() => { return this._meta.length > 1; });

        this._meta = meta;
        this._pipe_buffer = this._meta.pipe;
        this._rw_head = this._meta.rw_head;
        this._length = this._meta.length;
        this._begin = 0;
        this._end = this._meta.length;

        this._notifyOne = meta.notifyOne;
        this._notifyAll = meta.notifyAll;

        assert(() => {
            return this.validityOfIdx(this.readIdx()) &&
                this.validityOfIdx(this.writeIdx());
        });
    }

    private async declareMessage(msgType: string): Promise<MessageType> {
        const root = await load(protoPath(msgType) ?? "");
        return root.lookupType(msgType);
    }

    public notifyOne(): void {
        this._notifyOne();
    }

    public notifyAll(): void {
        this._notifyAll();
    }

    public async init(): Promise<void> {
        this._message = await this.declareMessage(this._meta.message_type);
    }

    public get length(): number {
        return this._length;
    }

    private readIdx(): number {
        return this._rw_head[0];
    }

    private setReadIdx(idx: number): void {
        this._rw_head[0] = idx;
    }

    private updateReadIdx(idx: number): void {
        this.setReadIdx(idx);
        this.notifyOne();
    }

    private writeIdx(): number {
        return this._rw_head[1];
    }

    private setWriteIdx(idx: number): void {
        this._rw_head[1] = idx;
    }

    private updateWriteIdx(idx: number): void {
        this.setWriteIdx(idx);
        this.notifyOne();
    }

    public isEmpty(): boolean {
        return this.readIdx() === this.writeIdx();
    }

    public isFull(): boolean {
        return this._length === 0 ||
            this.next(this.writeIdx()) === this.readIdx();
    }

    private next(current: number): number {
        const next_pos = current + 1;
        if (next_pos >= this._end) {
            return 0;
        } else {
            return next_pos;
        }
    }

    private nextN(current: number, steps: number): number {
        assert(() => { return this.validityOfIdx(current); })

        if (this._end - current - 1 >= steps) {
            return current + steps;
        } else {
            return this._begin + (steps - (this._end - current - 1)) - 1;
        }
    }

    private async waitNonfull(): Promise<void> {
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

    private async waitNonempty(): Promise<void> {
        // TODO: Same as waitNonfull()
        if (this.isEmpty()) {
            await waitFor(() => {
                return !this.isEmpty();
            });
        }
    }

    // This method may be blocked.
    public async read(): Promise<any | null> {
        assert(() => { return this.validityOfReadIdx(); });

        await this.waitNonempty();

        // Reading length field of message
        const rIdx = this.readIdx();
        const bytes_to_read = this._pipe_buffer[rIdx];
        assert(() => { return bytes_to_read > 0; });

        this.updateReadIdx(this.next(rIdx));

        return await this.readFromBuffer(bytes_to_read);
    }

    private async readFromBuffer(bytes_to_read: number): Promise<any | null> {
        const msg_buffer = new Uint8Array(bytes_to_read);

        let meta: ReadMeta = {
            r_idx: this.readIdx(),
            buffer_idx: 0,
            buffer: msg_buffer,
        };

        let remain = bytes_to_read;
        while (remain > 0) {
            let  bytes_in_buffer = Math.min(
                this.unreadBytesInBuffer(), remain);
            if (bytes_in_buffer == 0) {
                await this.waitNonempty();
                continue;
            }

            if (this.unreadBytesToEnd(meta.r_idx) > bytes_in_buffer) {
                const r_idx = meta.r_idx;
                msg_buffer.set(
                    this._pipe_buffer.subarray(r_idx, this._end),
                    r_idx);
                meta.r_idx = this.nextN(r_idx, bytes_in_buffer);
                meta.buffer_idx += bytes_in_buffer;
            } else {
                this.readCrossEnd(meta, bytes_in_buffer);
            }

            this.updateReadIdx(meta.r_idx);
            remain -= bytes_in_buffer;
        }
    }

    private async readCrossEnd(meta: ReadMeta, bytes_in_buffer: number) {
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

        meta.r_idx = this.nextN(r_idx, bytes_in_buffer);
        meta.buffer_idx = buffer_idx + bytes_in_buffer;
    }

    public async write(message: object): Promise<boolean> {

    }

    private validityOfIdx(idx: number): boolean {
        return this._begin <= idx && idx <= this._end;
    }

    private unreadBytesToEnd(idx: number): number {
        return this._end - idx;
    }

    private unreadBytesInBuffer(): number {
        const r_idx = this.readIdx(), w_idx = this.writeIdx();

        if (r_idx > w_idx) {
            return this._length - (r_idx - w_idx);
        } else {
            return w_idx - r_idx;
        }
    }

    private freeBytesInBuffer(): number {
        return this._length - this.unreadBytesInBuffer() - 1;
    }

    private freeBytesToEnd(idx: number) {
        return this._end - idx - 1;
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

    public read(): any | null {
        return this._in.read();
    }

    public write(message: object): boolean {
        return false;
    }

    public readable(): boolean {
        return !this._in.isEmpty();
    }

    public writable(): boolean {
        return !this._out.isFull();
    }
}
