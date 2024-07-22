import Module from "../src/eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
import * as native from "../src/eval/spaces/node_native_space/hello";

test("Napi Version", () => {
    expect(native != undefined).toBeTruthy();

    let size = 12
    let poly = new Float64Array(size);
    for (let i = 0; i < size; ++i) {
        poly[i] = size - i;
    }

    console.log(poly);
    native.insertionSort(poly);
    console.log(poly);


})

test("WASM Version", async () => {
    const mod = await Module();
    expect(mod != undefined).toBeTruthy();

    let size = 12
    let poly = new Float64Array(size);
    for (let i = 0; i < size; ++i) {
        poly[i] = size - i;
    }

    let buf: any = mod._malloc(
        poly.BYTES_PER_ELEMENT * poly.length);
    mod.HEAPF64.set(poly, buf / poly.BYTES_PER_ELEMENT);

    mod._insertionSortw(buf, poly.length, 1);
    mod._free(buf);
})
