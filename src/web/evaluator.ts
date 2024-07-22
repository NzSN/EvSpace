// import Module from "../eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
// import * as native from "../eval/spaces/node_native_space/hello";

import { is_nodejs } from "./platforms";

export function CreateEvaluator() {
    if (is_nodejs()) {

    } else {

    }
}
