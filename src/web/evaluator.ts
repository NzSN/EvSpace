import { BiPipe } from "./async/pipe";

export interface Evaluator {
    insertionSort(nums: Float64Array): void;
    asyncCount(nums: Float64Array): void;
    echo<T>(count: number, f: (number) => T): Promise<void>;
}
