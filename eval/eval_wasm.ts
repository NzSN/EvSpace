import { Eval } from "./eval";
import Module from './evalw';

export class WasmIsland implements Eval {
    private _mod: any;

    constructor() {}

    async init() {
        this._mod = await Module();
    }

    async Prerequisitions(): Promise<boolean> {
        await this.init();
        return true;
    }

    PolyFunc(nums: Float64Array): number {
        let buf: any = this._mod._malloc(
            nums.BYTES_PER_ELEMENT * nums.length);
        this._mod.HEAPF64.set(nums, buf / nums.BYTES_PER_ELEMENT);

        let result = this._mod._PolyFunction(buf, nums.length, 1);
        this._mod._free(buf);

        return result;
    }

    Sort(nums: Float64Array): void {
        let buf = this._mod._malloc(
            nums.BYTES_PER_ELEMENT * nums.length);
        this._mod.HEAPF64.set(nums, buf / nums.BYTES_PER_ELEMENT);

        this._mod._Sort(buf, nums.length);

        this._mod._free(buf);
    }

    SortWithDebug(nums: Float64Array): number {
         let buf = this._mod._malloc(
            nums.BYTES_PER_ELEMENT * nums.length);
        this._mod.HEAPF64.set(nums, buf / nums.BYTES_PER_ELEMENT);

        let elapsed = this._mod._SortWithDebug(buf, nums.length);

        this._mod._free(buf);

        return elapsed;
    }

    PthreadTest(): number {
        return 1;
    }
}
