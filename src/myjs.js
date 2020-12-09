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

mergeInto(LibraryManager.library, {
  sendToWindow: function (ptr) {
    const string = UTF8ToString(ptr);
    try {
      const data = window.JSON.parse(string);
      if (data === undefined || data === null) {
        return;
      }
      const event = new window.CustomEvent("updateFromSandbox", {
        detail: data,
      });
      window.dispatchEvent(event);
    } catch (_) {}
  },
  $extra__postset: "extra();",
  $extra: function () {
    const extra = window["sandboxExtra"];
    _setTimeout = (s, n) => extra["setTimeout"](UTF8ToString(s), n);
    _clearTimeout = (s, n) => extra["clearTimeout"](UTF8ToString(s), n);
    _setInterval = (s, n) => extra["setInterval"](UTF8ToString(s), n);
    _clearInterval = (s, n) => extra["clearInterval"](UTF8ToString(s), n);
    _alert = (s) => extra["alert"](UTF8ToString(s));
    _prompt = (q, d) =>
      stringToNewUTF8(extra["prompt"](UTF8ToString(q), UTF8ToString(d)));
    _parseURL = (s) => stringToNewUTF8(extra["parseURL"](UTF8ToString(s)));
    delete window["sandboxExtra"];
  },
  setTimeout: function () {},
  setTimeout__deps: ["$extra"],
  clearTimeout: function () {},
  clearTimeout__deps: ["$extra"],
  setInterval: function () {},
  setInterval__deps: ["$extra"],
  clearInterval: function () {},
  clearInterval__deps: ["$extra"],
  alert: function () {},
  alert__deps: ["$extra"],
  prompt: function () {},
  prompt__deps: ["$extra", "$stringToNewUTF8"],
  parseURL: function () {},
  parseURL__deps: ["$extra", "$stringToNewUTF8"],
  debugMe: function (ptr, alert) {
    const string = UTF8ToString(ptr);
    let data;
    try {
      data = window.JSON.parse(string);
    } catch (_) {
      data = string;
    }
    if (alert !== 0) {
      window.alert(string);
    } else {
      window.console.log("DEBUGME", data);
    }
  },
  printError: function (name_ptr, message_ptr, stack_ptr, alertOnError) {
    const name = UTF8ToString(name_ptr);
    const message = UTF8ToString(message_ptr);
    const stack = UTF8ToString(stack_ptr);
    const error = `Quickjs -- ${name}: ${message}\n${stack}`;
    if (alertOnError !== 0) {
      window.alert(error);
    } else {
      window.console.error(error);
    }
  },
  logMemUse: function (ptr) {
    const string = UTF8ToString(ptr);
    window.console.log(string);
  },
});
