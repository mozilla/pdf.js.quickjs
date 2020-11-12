/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#define DEBUG_ME 0
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
