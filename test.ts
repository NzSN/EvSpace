import Module from "./src/eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
import * as native from "./src/eval/spaces/node_native_space/hello";

test("Load module", () => {
    console.log(Module);
    console.log(native);
})
