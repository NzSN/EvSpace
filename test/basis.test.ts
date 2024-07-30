import Module from "../src/eval/spaces/wasm_space/wasm-space-cc/wasm-space.js";
import * as native from "../src/eval/spaces/node_native_space/hello";
import { delay } from "../src/web/utility.js";

test("Napi Version", async () => {
    expect(native != undefined).toBeTruthy();

    console.log(native);
    let meta = native.Echo();
    console.log(meta);
    await delay(2000);
    console.log(meta);

    let size = 12;
    let buffer = new SharedArrayBuffer(size * 8);
    let poly = new Float64Array(buffer);
    for (let i = 0; i < size; ++i) {
        poly[i] = size - i;
    }

    native.insertionSort(poly);

    for (let i = 1; i < size; ++i) {
        expect(poly[i-1] < poly[i]).toBeTruthy();
    }
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

    mod._insertionSortw(buf, poly.length);
    const array = new Float64Array(mod.HEAP8.buffer, buf, size);
    poly.set(array);

    for (let i = 1; i < size; ++i) {
        expect(poly[i-1] < poly[i]).toBeTruthy();
    }

    mod._free(buf);
})
