import { Evaluator } from './evaluator';
import * as basis from "../eval/spaces/node_native_space/hello";

export class NativeEvaluator implements Evaluator {
    insertionSort(nums: Float64Array) {
        basis.insertionSort(nums);
    }
}

export async function CreateEvaluator(): Promise<Evaluator> {
    return new NativeEvaluator();
}
