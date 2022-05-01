#!/bin/sh

OUTPUT=${OUTPUT:=.}
INPUT=${INPUT:=src}
QUICKJS=${QUICKJS:=./quickjs}

emcc -o ${OUTPUT}/quickjs-eval.js ${INPUT}/exports.c ${QUICKJS}/quickjs.c ${QUICKJS}/cutils.c ${QUICKJS}/libregexp.c ${QUICKJS}/libunicode.c -I${QUICKJS} -I${INPUT} -DCONFIG_VERSION="\"1.0.0\"" -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s SINGLE_FILE -s MODULARIZE=1 -s EXPORT_ES6=1 -s USE_ES6_IMPORT_META=0 -s ENVIRONMENT='web' -lm -Oz -s EXPORTED_FUNCTIONS='["_init", "_commFun", "_evalInSandbox", "_nukeSandbox", "_dumpMemoryUse", "_free"]' -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "stringToNewUTF8"]' -s AGGRESSIVE_VARIABLE_ELIMINATION=1 --closure 1 --js-library ${INPUT}/myjs.js
