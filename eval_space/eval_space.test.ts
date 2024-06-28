import { EVAL_ENV } from "../eval/eval_factory";
import { EvalSpace } from "./eval_space";
import * as polyfunc from "../eval/natives/polyfunc";

test("Polynomial function eval from wasm", async () => {
    let evalSpace: EvalSpace = new EvalSpace(EVAL_ENV.WASM);
    await evalSpace.Prerequisitions();

    const degree = 10e5;
    let poly = new Float64Array(degree);
    for (let i = 0; i < poly.length; ++i) {
        poly[i] = i;
    }

    let t0 = performance.now();
    let ret = await evalSpace.PolyFunc(poly);
    let t1 = performance.now();
    console.log(`${t1 - t0} ms`);
    console.log(`Result is ${ret}`);
});

test("Polynomial function eval from napi", async () => {
    let evalSpace: EvalSpace = new EvalSpace(EVAL_ENV.EXTENDED);
    await evalSpace.Prerequisitions();

    const degree = 10e5;
    let poly = new Float64Array(degree);
    for (let i = 0; i < poly.length; ++i) {
        poly[i] = i;
    }

    let t0 = performance.now();
    let ret = await evalSpace.PolyFunc(poly);
    let t1 = performance.now()
    console.log(`${t1 - t0} ms`);
    console.log(`Result is ${ret}`);
});
