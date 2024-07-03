export enum SPACES {
    WASM,
    NATIVE,
}

interface EvalSpace {
    sort(nums: Float64Array): void;
}

function BuildSpace(space: SPACES): EvalSpace {
    return undefined;
}
