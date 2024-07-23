import { Evaluator } from './evaluator';

export class WasmEvaluator implements Evaluator {
   
}

export function CreateEvaluator(): Evaluator {
    return new WasmEvaluator();
}
