import { EVAL_ENV, IslandFactory } from "../eval/eval_factory";

export class EvalSpace {
    tiedIsland: any;

    constructor(t: EVAL_ENV = EVAL_ENV.STANDARD) {
        this.tiedIsland = IslandFactory(t);
    }

    async Prerequisitions() {
        await this.tiedIsland.Prerequisitions();
    }

    async PolyFunc(nums: Float64Array) {
        return await this.tiedIsland.PolyFunc(nums);
    }
}
