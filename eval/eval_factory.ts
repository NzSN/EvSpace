import { WasmIsland } from './eval_wasm';
import { StandardIsland, Island } from './eval';
import { ExtendedIsland } from './eval_extended';

export enum EVAL_ENV {
    WASM,
    STANDARD,
    EXTENDED
}

export function IslandFactory(type: EVAL_ENV = EVAL_ENV.WASM): Island {
    switch (type) {
        case EVAL_ENV.WASM:
            return new WasmIsland();
        case EVAL_ENV.EXTENDED:
            return new ExtendedIsland();
        default:
            return new StandardIsland();
    }

}
