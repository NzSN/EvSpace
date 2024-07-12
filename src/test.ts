import Module from './eval/spaces/wasm_space/wasm-space-cc/wasm-space.js';
import * as Native from './eval/spaces/node_native_space/hello';

test("Simple", () => {
    console.log(Module);
    console.log(Native);
})
