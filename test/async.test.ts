import { RingPipeMeta, BiPipeMeta, BiPipe } from "../src/web/async/pipe";
import * as native from "../src/eval/spaces/node_native_space/node_native_space";
import { delay } from "../src/web/utility";

import { CreateEvaluator } from "../src/web/evaluator_factory";
import { NativeEvaluator } from "../src/web/evaluator_native";
import { WasmEvaluator } from "../src/web/evaluator_wasm";
import { Platform } from "../src/web/platforms";


test("Echo test", async () => {
    const mod = await native;

    let pipeMeta: BiPipeMeta = mod.Echo();

    let pipe: BiPipe = new BiPipe(pipeMeta);

    await pipe.init();

    let count = 20;
    let payload = { "counter" : 10 };

    while (count > 0) {
        payload.counter = count;
        await pipe.write(payload);

        let msg = await pipe.read();
        expect(msg != undefined).toBeTruthy();
        expect(msg?.counter == count).toBeTruthy();

        --count;
    }
})

test("Native Async", async () => {
    let count = 20;

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
