import { Eval } from "./eval";
import * as polyfunc from "../eval/natives/polyfunc";


export class ExtendedIsland implements Eval {
    Prerequisitions(): Promise<boolean> {
        return new Promise((resolve, _) => {
            resolve(true);
        });
    }

    async PolyFunc(nums: Float64Array): Promise<number> {
        return polyfunc.PolyFunction(nums);
    }
}
