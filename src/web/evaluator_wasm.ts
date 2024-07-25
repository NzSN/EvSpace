import { Evaluator } from './evaluator';
import Module from "../eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";

export class WasmEvaluator implements Evaluator {
    private _mod: any;

    async init() {
        this._mod = await Module();
    }

    insertionSort(nums: Float64Array) {
        let buf: any = this._mod._malloc(
            nums.BYTES_PER_ELEMENT * nums.length);
        this._mod.HEAPF64.set(nums, buf / nums.BYTES_PER_ELEMENT);

        this._mod._insertionSortw(buf, nums.length);
        const array = new Float64Array(this._mod.HEAP8.buffer, buf, nums.length);

        nums.set(array);
    }

    asyncCount(nums: Float64Array) {
        let buf: any = this._mod._malloc(
            nums.BYTES_PER_ELEMENT * nums.length);
        this._mod.HEAPF64.set(nums, buf / nums.BYTES_PER_ELEMENT);

        this._mod._asyncCountw(buf, nums.length);
        const array = new Float64Array(
            this._mod.HEAP8.buffer, buf, nums.length);

        return array;
    }
}

export async function CreateEvaluator(): Promise<Evaluator> {
    let evaluator = new WasmEvaluator();
    await evaluator.init();

    return evaluator;
}
