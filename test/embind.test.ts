import Module from "../src/eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
import { delay } from "../src/web/utility";
import * as protobuf from "protobufjs";
import { is_nodejs } from "../src/web/platforms";

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
    const counterSetterMessage = root.lookupType("CounterSetter");

    let pipe = mod.Communication();

    let count = getCounter(counterMessage, pipe);
    while (count < 60) {
        await delay(10);
        let updated = getCounter(counterMessage, pipe);
        expect(count <= updated).toBeTruthy();
        count = updated;
    }

    expect(pipe.message_type === "Counter").toBeTruthy();
    expect(count === 60).toBeTruthy();
})

test("Embind with IO", async () => {
    const mod = await Module();

    const root = await protobuf.load("src/eval/async/messages/counter.proto");
    const counterMessage = root.lookupType("Counter");
    const counterSetterMessage = root.lookupType("CounterSetter");

    let pipe = mod.MultiSettableCounter();
})
