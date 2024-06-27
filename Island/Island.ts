export interface IslandIFace {
    Prerequisitions(): Promise<boolean>;
    Computing(nums: Float64Array): Promise<number>;
}

export class StandardIsland implements IslandIFace {
    Prerequisitions(): Promise<boolean> {
        return new Promise((resolve, _) => {
            resolve(true);
        });
    }

    Computing(nums: Float64Array): Promise<number> {
        return new Promise((resolve, ) => {
            resolve(1);
        });
    }
}

export class ExtendedIsland implements IslandIFace {
    Prerequisitions(): Promise<boolean> {
        return new Promise((resolve, _) => {
            resolve(true);
        });
    }

    Computing(nums: Float64Array): Promise<number> {
        return new Promise((resolve, _) => {
            resolve(1);
        });
    }
}
