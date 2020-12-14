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
  $externals__postset: "externals();",
  $externals: function () {
    const externals = window["sandboxExternals"];
    _setTimeout = (id, n) => externals["setTimeout"](id, n);
    _clearTimeout = (id) => externals["clearTimeout"](id);
    _setInterval = (id, n) => externals["setInterval"](id, n);
    _clearInterval = (id) => externals["clearInterval"](id);
    _cleanTimeouts = () => externals["cleanTimeouts"]();
    _alert = (s) => externals["alert"](UTF8ToString(s));
    _prompt = (q, d) => {
      const response = externals["prompt"](UTF8ToString(q), UTF8ToString(d));
      if (response !== null) {
        return stringToNewUTF8(response);
      }
      return null;
    }
    _parseURL = (s) => stringToNewUTF8(externals["parseURL"](UTF8ToString(s)));
    _sendToWindow = (s) => externals["send"](UTF8ToString(s));
  },
  setTimeout: function () {},
  setTimeout__deps: ["$externals"],
  clearTimeout: function () {},
  clearTimeout__deps: ["$externals"],
  setInterval: function () {},
  setInterval__deps: ["$externals"],
  clearInterval: function () {},
  clearInterval__deps: ["$externals"],
  cleanTimeouts: function () {},
  cleanTimeouts__deps: ["$externals"],
  alert: function () {},
  alert__deps: ["$externals"],
  prompt: function () {},
  prompt__deps: ["$externals", "$stringToNewUTF8"],
  parseURL: function () {},
  parseURL__deps: ["$externals", "$stringToNewUTF8"],
  sendToWindow: function () {},
  sendToWindow__deps: ["$externals", "$stringToNewUTF8"],
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
