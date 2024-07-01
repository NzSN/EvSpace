export interface Eval {
    Prerequisitions(): Promise<boolean>;
    PolyFunc(nums: Float64Array): number;
    Sort(nums: Float64Array): void;
    SortWithDebug(nums: Float64Array): number;
    Pthreadtest(): number;
}
