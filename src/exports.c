/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quickjs.h"
#include "cutils.h"

static JSRuntime* runtime = NULL;
static JSContext* ctx = NULL;
static JSValue commFunction = 0;

static const char* getPropAsString(JSContext* ctx, JSValue obj, const char* name) {
    JSAtom prop = JS_NewAtom(ctx, name);
    JSValue val = JS_GetProperty(ctx, obj, prop);
    const char* str = JS_ToCString(ctx, val);
    JS_FreeValue(ctx, val);
    JS_FreeAtom(ctx, prop);

    return str;
}

extern void printError(const char*, const char*, const char*, int);
static BOOL buildAndPrintError(JSValueConst v, int alertOnError) {
    JSValue except;
    const char* name, *message, *stack;

    if (likely(!JS_IsException(v))) {
        return FALSE;
    }

    except = JS_GetException(ctx);
    name = getPropAsString(ctx, except, "name");
    if (!name) {
        JS_FreeValue(ctx, except);
        return TRUE;
    }

    stack = getPropAsString(ctx, except, "stack");
    if (!stack) {
        JS_FreeCString(ctx, name);
        JS_FreeValue(ctx, except);
        return TRUE;
    }

    message = getPropAsString(ctx, except, "message");
    if (!message) {
        JS_FreeCString(ctx, name);
        JS_FreeCString(ctx, stack);
        JS_FreeValue(ctx, except);
        return TRUE;
    }

    JS_FreeValue(ctx, except);

    printError(name, message, stack, alertOnError);
    JS_FreeCString(ctx, name);
    JS_FreeCString(ctx, stack);
    JS_FreeCString(ctx, message);

    return TRUE;
}

void evalInSandbox(const char* str, int alertOnError) {
    JSValue result;

    if (unlikely(!runtime)) {
        runtime = JS_NewRuntime();
        ctx = JS_NewContext(runtime);
    }

    result = JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
    buildAndPrintError(result, alertOnError);
    JS_FreeValue(ctx, result);
}

void nukeSandbox() {
    if (!runtime) {
        return;
    }
    if (commFunction) {
        JS_FreeValue(ctx, commFunction);
    }
    JS_FreeContext(ctx);
    ctx = NULL;
    JS_FreeRuntime(runtime);
    runtime = NULL;
}

BOOL initSandbox(const char* init, const char* cleanup, int alertOnError) {
    JSValue result;

    if (unlikely(runtime)) {
        nukeSandbox();
    }

    runtime = JS_NewRuntime();
    ctx = JS_NewContext(runtime);

    result = JS_Eval(ctx, init, strlen(init), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
    if (buildAndPrintError(result, alertOnError)) {
        JS_FreeValue(ctx, result);
        return FALSE;
    }

    commFunction = result;

    result = JS_Eval(ctx, cleanup, strlen(cleanup), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
    if (buildAndPrintError(result, alertOnError)) {
        JS_FreeValue(ctx, result);
        JS_FreeValue(ctx, commFunction);
        return FALSE;
    }

    JS_FreeValue(ctx, result);
    return TRUE;
}

void commFun(const char* name, const char* str, int alertOnError) {
    JSValue funName, result, arg;

    if (!commFunction) {
        return;
    }

    funName = JS_NewStringLen(ctx, name, strlen(name));

    result = JS_ParseJSON(ctx, str, strlen(str), "<commFun>");
    if (buildAndPrintError(result, alertOnError)) {
        JS_FreeValue(ctx, result);
        JS_FreeValue(ctx, funName);
        return;
    }

    JSValueConst args[2] = { funName, result };
    result = JS_Call(ctx, commFunction, JS_UNDEFINED, 2, args);
    JS_FreeValue(ctx, args[0]);
    JS_FreeValue(ctx, args[1]);

    buildAndPrintError(result, alertOnError);
    JS_FreeValue(ctx, result);
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
