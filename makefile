all: evalbox
evalbox: eval/EvalBox.c
	bear -- emcc -O3 -sWASM=1 -sFETCH=1 -sMODULARIZE=1 -sEXPORT_NAME=Eval -sEXPORTED_FUNCTIONS="['_malloc', '_free']" -o eval/EvalBox.js eval/EvalBox.c
deploy:
	npm run build
	mkdir -p build/eval
	cp eval/EvalBox.js build/eval/EvalBox.js
	cp eval/EvalBox.js build/eval/EvalBox.worker.js
	cp eval/EvalBox.wasm build/eval/EvalBox.wasm
