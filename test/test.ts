import Module from "../src/eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
import * as native from "../src/eval/spaces/node_native_space/hello";

test("Load module", async () => {
    const mod = await Module();
    expect(mod != undefined).toBeTruthy();
    expect(native != undefined).toBeTruthy();
})
