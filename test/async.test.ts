import { CreateEvaluator } from "../src/web/evaluator_factory";
import { Platform } from "../src/web/platforms";

test("Native Async", async () => {
    let count = 1000;

    let f = (reply: number) => {
        expect(count === reply).toBeTruthy();
        --count;
    };

    let evaluator = await CreateEvaluator(Platform.NATIVE);
    await evaluator.echo(count, f);
})

test("Wasm Async", async () => {
    let count = 20;

    let f = (reply: number) => {
        expect(count === reply).toBeTruthy();
        --count;
    };

    let evaluator = await CreateEvaluator(Platform.WASM);
    await evaluator.echo(count, f);

})
