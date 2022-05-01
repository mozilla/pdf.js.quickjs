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
extern const char* callExternalFunction(const char*, const char*, char* isError);
static JSValue js_callExternalFunction(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    const char *name, *str = NULL, *result;
    char isError = 0;
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

    result = callExternalFunction(name, str, &isError);
    JS_FreeCString(ctx, name);
    JS_FreeCString(ctx, str);

    if (!result) {
        return JS_UNDEFINED;
    }

    if (likely(!isError)) {
        obj = JS_ParseJSON(ctx, result, strlen(result), "<callExternalFunction>");
    } else {
        obj = JS_NewError(ctx);
        if (unlikely(JS_IsException(obj))) {
            /* out of memory: throw JS_NULL to avoid recursing */
            obj = JS_NULL;
        } else {
            JS_DefinePropertyValue(ctx, obj, JS_ATOM_message,
                                   JS_NewString(ctx, result),
                                   JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE);
        }

        obj = JS_Throw(ctx, obj);
    }
#ifdef free
#undef free
#endif
    free((void*)result);

    return obj;
}

extern double parseDate(const char*);
static JSValue js_Date_parse(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    double d;
    const char *s = JS_ToCString(ctx, argv[0]);
    if (!s) {
        return JS_EXCEPTION;
    }

    d = parseDate(s);
    JS_FreeCString(ctx, s);

    return JS_NewFloat64(ctx, d);
}

#define EXTRA CALLEXTERNALFUNCTION DUMP
