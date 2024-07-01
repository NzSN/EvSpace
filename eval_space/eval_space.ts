import { EVAL_ENV, IslandFactory } from "../eval/eval_factory";

export class EvalSpace {
    tiedIsland: any;

    constructor(t: EVAL_ENV = EVAL_ENV.STANDARD) {
        this.tiedIsland = IslandFactory(t);
    }

    async Prerequisitions(): Promise<boolean> {
        return await this.tiedIsland.Prerequisitions();
    }

    PolyFunc(nums: Float64Array) {
        return this.tiedIsland.PolyFunc(nums);
    }

    Sort(nums: Float64Array) {
        return this.tiedIsland.Sort(nums);
    }

    SortWithDebug(nums: Float64Array) {
        return this.tiedIsland.SortWithDebug(nums);
    }

    PthreadTest() {
        return this.tiedIsland.PthreadTest();
    }
}
