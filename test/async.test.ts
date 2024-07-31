import { RingPipeMeta, BiPipeMeta, BiPipe } from "../src/web/async/pipe";
import * as native from "../src/eval/spaces/node_native_space/hello";
import { delay } from "../src/web/utility";

test("Echo test", async () => {
    const mod = await native;

    let pipeMeta: BiPipeMeta = mod.Echo();

    let pipe: BiPipe = new BiPipe(pipeMeta);

    await pipe.init();

    let count = 20;
    let payload = { "counter" : 10 };

    while (count > 0) {
        payload.counter = count;
        pipe.write(payload);

        while (!pipe.readable()) {
            await delay(1);
        }
        let msg = pipe.read();

        expect(msg?.counter == count).toBeTruthy();

        --count;
    }

    payload.counter = 0;
    pipe.write(payload);
})
