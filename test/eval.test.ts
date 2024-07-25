import { Evaluator } from "../src/web/evaluator";
import { CreateEvaluator } from "../src/web/evaluator_factory"
import { Platform } from "../src/web/platforms";
import { delay } from "../src/web/utility";


let e: Evaluator | null = null
let size = 1024;
let nums: Float64Array | null = null;

function InsertionSortTest(e: Evaluator) {
    let nums_inited = nums as Float64Array;
    e?.insertionSort(nums_inited);
    for (let i = 1; i < size; ++i) {
        expect(nums_inited[i-1] < nums_inited[i]).toBeTruthy();
    }
}

beforeEach(async () => {
    nums = new Float64Array(size);
    for (let i = 0; i < size; ++i) {
        nums[i] = size - i;
    }
})

test("AutoSelect Evaluator", async () => {
    e = await CreateEvaluator();
    InsertionSortTest(e as Evaluator);
})

test("Native Evaluator", async () => {
    e = await CreateEvaluator(Platform.NATIVE)
    InsertionSortTest(e as Evaluator);
})

test("WASM Evaluator", async () => {
    e = await CreateEvaluator(Platform.WASM)
    InsertionSortTest(e as Evaluator);
})


async function asyncCount(e: Evaluator) {
    let nums_inited = nums as Float64Array;
    nums_inited[0] = 0;

    let countDelay = 50;
    let maximum = 10;
    nums_inited[1] = countDelay;
    nums_inited[2] = maximum;
    e.asyncCount(nums_inited);

    let num = 0;
    while (nums_inited[0] < 60) {
        num = nums_inited[0];
        await delay(maximum);
        expect(num <= nums_inited[0]).toBeTruthy();
    }
}

async function asyncCountTestWASM(e: Evaluator) {
    let nums_inited = nums as Float64Array;
    nums_inited[0] = 0;

    let countDelay = 10;
    let maximum = 60;
    nums_inited[1] = countDelay;
    nums_inited[2] = maximum;
    let array = e.asyncCount(nums_inited);

    let num = 0;
    while (array[0] < 60) {
        num = array[0];
        await delay(maximum);
        expect(num <= array[0]).toBeTruthy();
    }
}

test("Native Evaluator (asyncCount)", async () => {
    e = await CreateEvaluator(Platform.NATIVE);
    await asyncCount(e as Evaluator);
})

test("WASM Evaluator (asyncCount)", async () => {
    e = await CreateEvaluator(Platform.WASM);
    await asyncCountTestWASM(e as Evaluator);
})
