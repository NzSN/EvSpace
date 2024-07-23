// import Module from "../eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
// import * as native from "../eval/spaces/node_native_space/hello";

import { is_nodejs } from "./platforms";

async function loadModule() {
    return is_nodejs() ?
        await import("./evaluator_native") :
        await import("./evaluator_wasm");
}

export async function CreateEvaluator() {
    const module = await loadModule();
    return module.CreateEvaluator();
}
