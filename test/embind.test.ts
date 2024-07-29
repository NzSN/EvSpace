import * as protobuf from "protobufjs";

import Module from "../src/eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
import { delay } from "../src/web/utility";
import { is_nodejs } from "../src/web/platforms";
import { RingPipeMeta, BiPipeMeta, BiPipe } from "../src/web/async/pipe";

function getCounter(messenger: any, pipe: any): number {
    let msg: any;

    if (is_nodejs()) {
        const buffer = Buffer.from(pipe.pipe);
        msg = messenger.decode(buffer);
    } else {
        msg = messenger.decode(pipe.pipe);
    }

    return msg.counter;
}

test("Embind test", async () => {
    const mod = await Module();
    const root = await protobuf.load("src/eval/async/messages/counter.proto");
    const counterMessage = root.lookupType("Counter");

    let pipe = mod.Communication();

    let count = getCounter(counterMessage, pipe);
    while (count < 60) {
        await delay(5);
        let updated = getCounter(counterMessage, pipe);
        expect(count <= updated).toBeTruthy();
        count = updated;
    }

    expect(pipe.message_type === "Counter").toBeTruthy();
    expect(count === 60).toBeTruthy();
})

test("Echo test", async () => {
    const mod = await Module();

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
