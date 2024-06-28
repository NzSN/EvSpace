all: evalbox
evalbox: Island/EvalBox.c
	bear -- emcc -O2 -sWASM=1 -sFETCH=1 -sMODULARIZE=1 -sEXPORT_NAME=Eval -sEXPORTED_FUNCTIONS="['_malloc', '_free']" -o Island/EvalBox.js Island/EvalBox.c
deploy:
	npm run build
	mkdir -p build/Island
	cp Island/EvalBox.js build/Island/EvalBox.js
	cp Island/EvalBox.js build/Island/EvalBox.worker.js
	cp Island/EvalBox.wasm build/Island/EvalBox.wasm
