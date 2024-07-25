
import { is_nodejs, Platform } from "./platforms";

async function loadModule(plat: Platform) {
    if (plat === Platform.AUTO) {
        return is_nodejs() ?
            await import("./evaluator_native") :
            await import("./evaluator_wasm");
    } else if (plat === Platform.NATIVE) {
        return await import("./evaluator_native");
    } else {
        return await import("./evaluator_wasm");
    }
}

export async function CreateEvaluator(plat: Platform = Platform.AUTO) {
    const module = await loadModule(plat);
    return await module.CreateEvaluator();
}
