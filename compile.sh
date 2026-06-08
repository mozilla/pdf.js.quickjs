#!/bin/sh

set -e

OUTPUT=${OUTPUT:=.}
INPUT=${INPUT:=src}
QUICKJS=${QUICKJS:=./quickjs}
OUTPUT_FILE=${OUTPUT_FILE:=quickjs-eval.js}

emcc -o "${OUTPUT}/${OUTPUT_FILE}" \
        "${INPUT}/exports.c" \
        "${QUICKJS}/quickjs.c" \
        "${QUICKJS}/cutils.c" \
        "${QUICKJS}/libregexp.c" \
        "${QUICKJS}/libunicode.c" \
        "${QUICKJS}/dtoa.c" \
        -I"${QUICKJS}" \
        -I"${INPUT}" \
        -DCONFIG_VERSION="\"1.0.0\"" \
        -DEMSCRIPTEN \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s MAXIMUM_MEMORY=2GB \
        -s STACK_SIZE=1048576 \
        -s WASM=1 \
        -s MODULARIZE=1 \
        -s EXPORT_NAME="'QuickJS'" \
        -s WASM_ASYNC_COMPILATION=1 \
        -s EXPORT_ES6=1 \
        -s SINGLE_FILE=0 \
        -s ENVIRONMENT='web' \
        -s ERROR_ON_UNDEFINED_SYMBOLS=1 \
        -s NO_FILESYSTEM=1 \
        -s NO_EXIT_RUNTIME=1 \
        -s MALLOC=emmalloc \
        -lm -Oz \
        -s EXPORTED_FUNCTIONS='["_init", "_commFun", "_evalInSandbox", "_nukeSandbox", "_dumpMemoryUse", "_free"]' \
        -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "stringToNewUTF8"]' \
        -s ASSERTIONS=0 \
        -DNDEBUG \
        -flto \
        --closure 1 \
        --js-library "${INPUT}/myjs.js"

chmod ugo-x "${OUTPUT}/quickjs-eval.wasm"
sed -i '1 i\/* THIS FILE IS GENERATED - DO NOT EDIT */' "${OUTPUT}/${OUTPUT_FILE}"
