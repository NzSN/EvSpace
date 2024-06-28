import { EVAL_ENV } from "../eval/eval_factory";
import { EvalSpace } from "./eval_space";
import * as polyfunc from "../eval/natives/polyfunc";

test("Polynomial functions eval", async () => {
    let evalSpace: EvalSpace = new EvalSpace(EVAL_ENV.WASM);
    await evalSpace.Prerequisitions();

    const degree = 10;
    let poly = new Float64Array(degree);
    for (let i = 0; i < poly.length; ++i) {
        poly[i] = i;
    }

    expect(await evalSpace.PolyFunc(poly) == 45).toBeTruthy();
});

test("NAPI", async () => {
    let poly = new Float64Array(10);
    for (let i = 0; i < poly.length; ++i) {
        poly[i] = i;
    }

    expect(polyfunc.PolyFunction(poly) == 45).toBeTruthy();
});
