import { Island } from "./Island";
import Module from './EvalBox';

export class WasmIsland implements Island {
    private _mod: any;

    constructor() {}

    async init() {
        this._mod = await Module();
    }

    async Prerequisitions(): Promise<boolean> {
        await this.init();
        return true;
    }

    async PolyFunc(nums: Float64Array): Promise<number> {
        let buf: any = this._mod._malloc(
            nums.BYTES_PER_ELEMENT * nums.length);
        this._mod.HEAPF64.set(nums, buf / nums.BYTES_PER_ELEMENT);

        let result = this._mod._PolyFunction(buf, nums.length, 1);
        this._mod._free(buf);

        return result;
    }
}
