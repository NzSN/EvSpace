all: evalbox
evalbox: eval/wasms/poly.cc eval/wasms/sort.cc
	bear -- em++ -pthread -O2 -sWASM=1 -sFETCH=1 -sMODULARIZE=1 -sEXPORT_NAME=Eval -sEXPORTED_FUNCTIONS="['_malloc', '_free']" -o eval/evalw.js eval/wasms/poly.cc eval/wasms/sort.cc eval/evals/poly.cc eval/evals/sort.cc
deploy:
	npm run build
	mkdir -p build/eval
	cp eval/EvalBox.js build/eval/EvalBox.js
	cp eval/EvalBox.js build/eval/EvalBox.worker.js
	cp eval/EvalBox.wasm build/eval/EvalBox.wasm
