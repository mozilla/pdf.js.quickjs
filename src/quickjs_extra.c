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

#define DUMP JS_CFUNC_DEF("dump", 2, js_dump ),
extern void dump(const char*, const int);
static JSValue js_dump(JSContext *ctx, JSValueConst this_val,
                       int argc, JSValueConst *argv)
{
    const char *str;
    int alert = 0;
    JSValue json;

    if (argc < 1 || argc > 2) {
        return JS_UNDEFINED;
    }

    json = JS_JSONStringify(ctx, argv[0], JS_UNDEFINED, JS_UNDEFINED);
    if (JS_IsException(json)) {
        return json;
    }
    str = JS_ToCString(ctx, json);
    JS_FreeValue(ctx, json);
    if (!str) {
        return JS_EXCEPTION;
    }

    if (argc == 2) {
        alert = JS_ToBool(ctx, argv[1]);
    }

    dump(str, alert);

    JS_FreeCString(ctx, str);

    return JS_UNDEFINED;
}

#define CALLEXTERNALFUNCTION JS_CFUNC_DEF("callExternalFunction", 2, js_callExternalFunction),
extern const char* callExternalFunction(const char*, const char*);
static JSValue js_callExternalFunction(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    const char *name, *str = NULL, *result;
    JSValue json, obj;

    if (argc < 1 || argc > 2) {
        return JS_UNDEFINED;
    }

    name = JS_ToCString(ctx, argv[0]);
    if (!name) {
        return JS_EXCEPTION;
    }

    if (argc == 2) {
        json = JS_JSONStringify(ctx, argv[1], JS_UNDEFINED, JS_UNDEFINED);
        if (JS_IsException(json)) {
            JS_FreeCString(ctx, name);
            return json;
        }
        str = JS_ToCString(ctx, json);
        JS_FreeValue(ctx, json);
        if (!str) {
            JS_FreeCString(ctx, name);
            return JS_EXCEPTION;
        }
    }

    result = callExternalFunction(name, str);
    JS_FreeCString(ctx, name);
    JS_FreeCString(ctx, str);

    if (!result) {
        return JS_UNDEFINED;
    }

    obj = JS_ParseJSON(ctx, result, strlen(result), "<output>");
#ifdef free
#undef free
#endif
    free((void*)result);

    return obj;
}

#define EXTRA CALLEXTERNALFUNCTION DUMP
