/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quickjs.h"

static JSRuntime* runtime = NULL;
static JSContext* ctx = NULL;

void evalInSandbox(const char* str) {
    JSValue result;

    if (!runtime) {
        runtime = JS_NewRuntime();
        ctx = JS_NewContext(runtime);
    }

    result = JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
    JS_FreeValue(ctx, result);
}

extern void clearIds();
void nukeSandbox() {
    if (!runtime) {
        return;
    }
    clearIds();
    JS_FreeContext(ctx);
    ctx = NULL;
    JS_FreeRuntime(runtime);
    runtime = NULL;
}

extern void logMemUse(const char*);
void dumpMemoryUse() {
    JSMemoryUsage stats;
    char *output;
    FILE *file;
    
    if (!runtime) {
        return;
    }
    
    output = (char*)malloc(sizeof(char) * 1024);
    file = fmemopen(output, 1024, "w");

    JS_ComputeMemoryUsage(runtime, &stats);
    JS_DumpMemoryUsage(file, &stats, runtime);
    fclose(file);

    logMemUse(output);
    
    free((void*)output);
}
