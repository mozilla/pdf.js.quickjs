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

#define DEBUG_ME 1
#if DEBUG_ME != 0
#define DEBUGME JS_CFUNC_DEF("debugMe", 2, js_debugMe ),
extern void debugMe(const char*, const int);
static JSValue js_debugMe(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    const char *str;
    int alert = 0;
    JSValue json;

    if (argc < 1 || argc > 2) {
        return JS_EXCEPTION;
    }

    json = JS_JSONStringify(ctx, argv[0], JS_UNDEFINED, JS_UNDEFINED);
    if (JS_IsException(json)) {
        return JS_EXCEPTION;
    }
    str = JS_ToCString(ctx, json);
    JS_FreeValue(ctx, json);
    if (!str) {
        return JS_EXCEPTION;
    }

    if (argc == 2) {
        alert = JS_ToBoolFree(ctx, argv[1]);
    }

    debugMe(str, alert);

    JS_FreeCString(ctx, str);

    return JS_UNDEFINED;
}
#else
#define DEBUGME
#endif

#define SEND JS_CFUNC_DEF("send", 1, js_send),
extern void sendToWindow(const char*);
static JSValue js_send(JSContext *ctx, JSValueConst this_val,
                       int argc, JSValueConst *argv)
{
    const char *str;
    JSValue json = JS_JSONStringify(ctx, argv[0], JS_UNDEFINED, JS_UNDEFINED);
    if (JS_IsException(json)) {
        return JS_EXCEPTION;
    }
    str = JS_ToCString(ctx, json);
    JS_FreeValue(ctx, json);

    sendToWindow(str);

    JS_FreeCString(ctx, str);

    return JS_UNDEFINED;
}

#define SETTIMEOUT JS_CFUNC_DEF("setTimeout", 2, js_setTimeout),
extern int setTimeout(const char*, double);
static JSValue js_setTimeout(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    const char *str;
    double millisecs = 0.;
    int id;

    if (argc < 1 || argc > 2) {
        return JS_EXCEPTION;
    }

    if (argc == 2 && JS_ToFloat64(ctx, &millisecs, argv[1])) {
        return JS_EXCEPTION;
    }

    str = JS_ToCString(ctx, argv[0]);
    if (!str) {
        return JS_EXCEPTION;
    }

    id = setTimeout(str, millisecs);
    JS_FreeCString(ctx, str);

    return JS_NewInt32(ctx, id);
}

#define CLEARTIMEOUT JS_CFUNC_DEF("clearTimeout", 1, js_clearTimeout),
extern void clearTimeout(int);
static JSValue js_clearTimeout(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    int id;

    if (argc != 1) {
        return JS_EXCEPTION;
    }

    if (JS_ToInt32(ctx, &id, argv[0])) {
        return JS_EXCEPTION;
    }
    clearTimeout(id);

    return JS_UNDEFINED;
}

#define SETINTERVAL JS_CFUNC_DEF("setInterval", 2, js_setInterval),
extern int setInterval(const char*, double);
static JSValue js_setInterval(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv)
{
    const char *str;
    double millisecs = 0.;
    int id;

    if (argc < 1 || argc > 2) {
        return JS_EXCEPTION;
    }

    if (argc == 2 && JS_ToFloat64(ctx, &millisecs, argv[1])) {
        return JS_EXCEPTION;
    }

    str = JS_ToCString(ctx, argv[0]);
    if (!str) {
        return JS_EXCEPTION;
    }
    id = setInterval(str, millisecs);
    JS_FreeCString(ctx, str);

    return JS_NewInt32(ctx, id);
}

#define CLEARINTERVAL JS_CFUNC_DEF("clearInterval", 1, js_clearInterval),
extern void clearInterval(int);
static JSValue js_clearInterval(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    int id;

    if (argc != 1) {
        return JS_EXCEPTION;
    }

    if (JS_ToInt32(ctx, &id, argv[0])) {
        return JS_EXCEPTION;
    }
    clearInterval(id);

    return JS_UNDEFINED;
}

#define CRACKURL JS_CFUNC_DEF("crackURL", 1, js_crackURL),
extern char* crackURL(const char*);
static JSValue js_crackURL(JSContext *ctx, JSValueConst this_val,
                           int argc, JSValueConst *argv)
{
    const char *str;
    const char *result;
    size_t resultLen;
    JSValue obj;

    if (argc != 1) {
        return JS_EXCEPTION;
    }

    str = JS_ToCString(ctx, argv[0]);
    if (!str) {
        return JS_EXCEPTION;
    }

    result = crackURL(str);
    JS_FreeCString(ctx, str);
    resultLen = strlen(result);

    obj = JS_ParseJSON(ctx, result, resultLen, "<input>");
    if (JS_IsException(obj)) {
        JS_FreeValue(ctx, obj);
        obj = JS_ThrowTypeError(ctx, "%s", result);
    }

#ifdef free
#undef free
#endif
    free((void*)result);

    return obj;
}

#define EXTRA SEND SETTIMEOUT CLEARTIMEOUT SETINTERVAL CLEARINTERVAL CRACKURL DEBUGME
