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
        JS_FreeValue(ctx, json);
        return JS_EXCEPTION;
    }
    str = JS_ToCString(ctx, json);
    JS_FreeValue(ctx, json);
    if (!str) {
        return JS_EXCEPTION;
    }

    if (argc == 2) {
        alert = JS_ToBool(ctx, argv[1]);
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
        JS_FreeValue(ctx, json);
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

#define PARSEURL JS_CFUNC_DEF("parseURL", 1, js_parseURL),
extern char* parseURL(const char*);
static JSValue js_parseURL(JSContext *ctx, JSValueConst this_val,
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

    result = parseURL(str);
    JS_FreeCString(ctx, str);
    resultLen = strlen(result);

    obj = JS_ParseJSON(ctx, result, resultLen, "<input>");
#ifdef free
#undef free
#endif
    free((void*)result);

    if (JS_IsException(obj)) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }

    return obj;
}

#define ALERT JS_CFUNC_DEF("alert", 1, js_alert),
extern void alert(const char*);
static JSValue js_alert(JSContext *ctx, JSValueConst this_val,
                        int argc, JSValueConst *argv)
{
    const char *str;

    if (argc != 1) {
        return JS_EXCEPTION;
    }

    str = JS_ToCString(ctx, argv[0]);
    if (!str) {
        return JS_EXCEPTION;
    }

    alert(str);

    JS_FreeCString(ctx, str);

    return JS_UNDEFINED;
}

#define PROMPT JS_CFUNC_DEF("prompt", 2, js_prompt),
extern const char* prompt(const char*, const char*);
static JSValue js_prompt(JSContext *ctx, JSValueConst this_val,
                         int argc, JSValueConst *argv)
{
    const char *quest, *def, *res;
    JSValue val;

    if (argc < 1 || argc > 2) {
        return JS_EXCEPTION;
    }

    quest = JS_ToCString(ctx, argv[0]);
    if (!quest) {
        return JS_EXCEPTION;
    }

    if (argc == 2) {
        def = JS_ToCString(ctx, argv[1]);
        if (!def) {
            JS_FreeCString(ctx, quest);
            return JS_EXCEPTION;
        }
        res = prompt(quest, def);
        JS_FreeCString(ctx, def);
    } else {
        res = prompt(quest, "");
    }

    JS_FreeCString(ctx, quest);
    val = JS_NewStringLen(ctx, res, strlen(res));

#ifdef free
#undef free
#endif
    free((void*)res);

    return val;
}

#define GLOBALEVAL JS_CFUNC_DEF("globalEval", 1, js_globalEval),
static JSValue js_globalEval(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    const char *str;
    JSValue result;

    if (argc != 1) {
        return JS_EXCEPTION;
    }

    str = JS_ToCString(ctx, argv[0]);
    if (!str) {
        return JS_EXCEPTION;
    }

    result = JS_Eval(ctx, str, strlen(str), "<eval>", JS_EVAL_TYPE_GLOBAL);
    JS_FreeCString(ctx, str);

    return result;
}

#define EXTRA SEND SETTIMEOUT CLEARTIMEOUT SETINTERVAL CLEARINTERVAL PARSEURL ALERT PROMPT DEBUGME GLOBALEVAL
