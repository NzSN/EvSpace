import { IslandIFace } from "./Island";
import Module from './EvalBox';

export class WasmIsland implements IslandIFace {
    private _mod: any;

    constructor() {}

    async init() {
        this._mod = await Module();
    }

    async Prerequisitions(): Promise<boolean> {
        await this.init();
        return true;
    }

    async Computing(nums: Float64Array): Promise<number> {
        return this._mod._Sort();
    }
}
