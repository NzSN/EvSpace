import { Eval } from "./eval";
import * as polyfunc from "../eval/natives/evaln";


export class ExtendedIsland implements Eval {
    Prerequisitions(): Promise<boolean> {
        return new Promise((resolve, _) => {
            resolve(true);
        });
    }

    PolyFunc(nums: Float64Array): number {
        return polyfunc.PolyFunction(nums);
    }

    Sort(nums: Float64Array): number {
        return polyfunc.Sort(nums);
    }

    SortWithDebug(nums: Float64Array): number {
        return polyfunc.SortWithDebug(nums);
    }

    PthreadTest(): number {
        return polyfunc.PthreadTest();
    }
}
