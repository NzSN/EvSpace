/* Structures defined here are isomorphic to some structures defined in C++ realm.
 * Particularly, you can't create information from C++ realm. The purposes
 * of these structures is to exchange information from C++ realm */
import { load, Type as MessageType, Message } from "protobufjs";
import { protoPath } from "./message";

/* Metadata return from C++ Realm */
export interface RingPipeMeta {
    message_type : string;
    pipe         : Uint8Array;
    rw_head      : Uint32Array;
    length       : number;
}
export interface BiPipeMeta {
    in  : RingPipeMeta;
    out : RingPipeMeta;
}

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
        this._pipe_buffer = this._meta.pipe;
        this._rw_head = this._meta.rw_head;
        this._length = this._meta.length;
        this._begin = 0;
        this._end = this._meta.length;
    }

    private async declareMessage(msgType: string): Promise<MessageType> {
        const root = await load(protoPath(msgType) ?? "");
        return root.lookupType(msgType);
    }

    public async init() {
        this._message = await this.declareMessage(this._meta.message_type);
    }

    public get length() {
        return this._length;
    }

    private read_idx() {
        return this._rw_head[0];
    }

    private set_read_idx(idx: number) {
        this._rw_head[0] = idx;
    }

    private write_idx() {
        return this._rw_head[1];
    }

    private set_write_idx(idx: number) {
        this._rw_head[1] = idx;
    }

    public isEmpty(): boolean {
        return this.read_idx() === this.write_idx();
    }

    public isFull(): boolean {
        return this._length === 0 ||
            this.next(this.write_idx()) === this.read_idx();
    }

    private next(current: number): number {
        const next_pos = current + 1;
        if (next_pos >= this._end) {
            return 0;
        } else {
            return next_pos;
        }
    }

    public read(): any | null {
        const message = this.peek();
        this.set_read_idx(this.next(this.read_idx()));

        return message;
    }

    public write(message: object): boolean {
        if (this.isFull()) {
            return false;
        }

        const buffer = this._message.encode(message).finish();

        if (buffer.length != this._size) {
            throw new Error(
                "Unable to write message into buffer: length not match");
        }

        this._pipe_buffer.set(buffer, this.write_idx() * this._size);
        this.set_write_idx(this.next(this.write_idx()));

        return true;
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

    public read(): any | null {
        return this._in.read();
    }

    public write(message: object): boolean {
        return this._out.write(message);
    }

    public peek(): Message<{}> | null {
        return this._in.peek();
    }

    public readable(): boolean {
        return !this._in.isEmpty();
    }

    public writable(): boolean {
        return !this._out.isFull();
    }
}
