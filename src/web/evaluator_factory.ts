
import { is_nodejs } from "./platforms";

async function loadModule() {
    return is_nodejs() ?
        await import("./evaluator_native") :
        await import("./evaluator_wasm");
}

export async function CreateEvaluator() {
    const module = await loadModule();
    return await module.CreateEvaluator();
}
