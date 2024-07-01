import { EVAL_ENV } from "../eval/eval_factory";
import { EvalSpace } from "./eval_space";
import * as polyfunc from "../eval/natives/evaln";


let wasmSpace: EvalSpace;
let extendedSpace: EvalSpace;

beforeEach(async () => {
    wasmSpace = new EvalSpace(EVAL_ENV.WASM);
    await wasmSpace.Prerequisitions();

    extendedSpace = new EvalSpace(EVAL_ENV.EXTENDED);
    await wasmSpace.Prerequisitions();
});

test("Polynomial function eval", async () => {
    const degree = 10e5;
    let poly = new Float64Array(degree);
    for (let i = 0; i < poly.length; ++i) {
        poly[i] = i;
    }

    let t0 = performance.now();
    let r0 = wasmSpace.PolyFunc(poly);
    let t1 = performance.now();
    let elapsed_wasm = t1 - t0;

    t0 = performance.now();
    let r1 = extendedSpace.PolyFunc(poly);
    t1 = performance.now();
    let elapsed_ext = t1 - t0;

    expect(r1 == r0).toBeTruthy();

    console.log(`Poly: WASM(${elapsed_wasm})  EXT(${elapsed_ext})`);
});

test("Sort", async () => {
    let count = 10000;
    let nums = new Float64Array(count);
    for (let i = 0; i < nums.length; ++i) {
        nums[i] = nums.length - i;
    }

    let t0 = performance.now();
    wasmSpace.Sort(nums);
    let t1 = performance.now();
    let elapsed_wasm = t1 - t0;

    t0 = performance.now();
    extendedSpace.Sort(nums);
    t1 = performance.now();
    let elapsed_ext = t1 - t0;
    console.log(`Sort: WASM(${elapsed_wasm})  EXT(${elapsed_ext})`);
});

test("Multithread", async () => {
    console.log(extendedSpace.PthreadTest());
});
