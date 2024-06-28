import { Eval } from "./eval";


export class ExtendedIsland implements Eval {
    Prerequisitions(): Promise<boolean> {
        return new Promise((resolve, _) => {
            resolve(true);
        });
    }

    PolyFunc(nums: Float64Array): Promise<number> {
        return new Promise((resolve, _) => {
            resolve(1);
        });
    }
}
