import { IslandFactory, ISLAND_TYPE, WASM } from "../Island/IslandFactory";

export class Mainland {
    tiedIsland: any;

    constructor(t: ISLAND_TYPE = WASM) {
        this.tiedIsland = IslandFactory(t);
    }
    async Computing() {
        return await this.tiedIsland.Computing();
    }
}
