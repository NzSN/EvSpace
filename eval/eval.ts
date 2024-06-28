export interface Eval {
    Prerequisitions(): Promise<boolean>;
    PolyFunc(nums: Float64Array): Promise<number>;
}

export class StandardIsland implements Eval {
    Prerequisitions(): Promise<boolean> {
        return new Promise((resolve, _) => {
            resolve(true);
        });
    }

    PolyFunc(nums: Float64Array): Promise<number> {
        return new Promise((resolve, ) => {
            resolve(1);
        });
    }
}
