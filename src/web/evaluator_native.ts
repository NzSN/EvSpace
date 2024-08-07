import { Evaluator } from './evaluator';
import * as basis from "../eval/spaces/node_native_space/node_native_space";
import { BiPipe, BiPipeMeta } from './async/pipe';
import { delay } from './utility';

export class NativeEvaluator implements Evaluator {
    insertionSort(nums: Float64Array) {
        basis.insertionSort(nums);
    }
    asyncCount(nums: Float64Array) {
        basis.asyncCount(nums);
    }

    async echo<T>(count: number, f: (number) => T): Promise<void> {
        let pipeMeta: BiPipeMeta = basis.Echo();
        let pipe: BiPipe = new BiPipe(pipeMeta);

        await pipe.init();

        let payload = { "counter" : 0 };
        while (count > 0) {
            payload.counter = count;
            await pipe.write(payload);

            const msg = await pipe.read();

            f(msg?.counter);

            --count;
        }

        payload.counter = 0;
        pipe.write(payload);
    }
}

export async function CreateEvaluator(): Promise<Evaluator> {
    return new NativeEvaluator();
}
