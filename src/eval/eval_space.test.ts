import { EvalSpace, SPACES } from './eval_space';

test("EvalSpace", () => {
    let evSpace = BuildSpace(SPACES.WASM);

    const count = 10000;
    let nums = new Float64Array(count);
    for (let i = 0; i < count; ++i) {
        nums[i] = count - i;
    }
    evSpace.sort(nums);

    for (let i = 1; i < count; ++i) {
        expect(nums[i] > nums[i-1]).toBeTruthy();
    }
});
