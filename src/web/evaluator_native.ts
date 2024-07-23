import { Evaluator } from './evaluator';

export class NativeEvaluator implements Evaluator {
   
}

export function CreateEvaluator(): Evaluator {
    return new NativeEvaluator();
}
