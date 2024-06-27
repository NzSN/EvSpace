import { WasmIsland } from './WasmIsland';
import { StandardIsland, ExtendedIsland } from './Island';
import { strict as assert } from 'assert';

export class ISLAND_TYPE{}
export class WASM implements ISLAND_TYPE{}
export class STANDARD implements ISLAND_TYPE{}
export class EXTENDED implements ISLAND_TYPE{}

type IMPL_OF_ISLAND_TYPE<T extends ISLAND_TYPE> =
    T extends WASM ? WasmIsland :
    T extends STANDARD ? StandardIsland :
    T extends EXTENDED ? ExtendedIsland :
    never;


export class Island<T extends ISLAND_TYPE> {
    private _impl: IMPL_OF_ISLAND_TYPE<T>;

    constructor(ctor: { new (): IMPL_OF_ISLAND_TYPE<T> }) {
        this._impl = new ctor();

        this._impl.Prerequisitions().then((satisified: boolean) => {
            assert(satisified);
        });
    }

    Computing(nums: Float64Array) {
        return this._impl.Computing(nums);
    }
}

export function IslandFactory(t: ISLAND_TYPE = WASM): Island<typeof t> {
    if (t instanceof WasmIsland) {
        return new Island<typeof t>(WasmIsland);
    } else if (t instanceof STANDARD) {
        return new Island<typeof t>(StandardIsland as any);
    } else {
        return new Island<typeof t>(ExtendedIsland as any);
    }
}
