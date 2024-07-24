import { Evaluator } from "../src/web/evaluator";
import { CreateEvaluator } from "../src/web/evaluator_factory"
import { is_nodejs } from "../src/web/platforms";

let e: Evaluator | null = null

beforeEach(async () => {
    e = await CreateEvaluator();
})

test("Evaluator", async () => {
    let nums = new Float64Array(12);
    for (let i = 0; i < 12; ++i) {
        nums[i] = 12 - i;
    }

    e?.insertionSort(nums);

    for (let i = 1; i < 12; ++i) {
        expect(nums[i-1] < nums[i]).toBeTruthy();
    }
})
