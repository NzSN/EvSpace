import { WASM } from "../Island/IslandFactory";
import { Mainland } from "./mainland";

test("Mainland", async () => {
    let mainland: Mainland = new Mainland(WASM);

    expect(await mainland.Computing() == 1).toBeTruthy();
});
