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
  sendToWindow: function(ptr) {
    const string = UTF8ToString(ptr);
    try {
      const data = window.JSON.parse(string);
      if (data === undefined || data === null) {
        return;
      }
      const event = new window.CustomEvent("updateFromSandbox", { detail: data });
      window.dispatchEvent(event);
    } catch (_) {}
  },
  $timeout__postset: "timeout();",
  $timeout: function() {
    class Timeout {
      constructor() {
        this.timeoutIds = new Set();
      }

      setTimeout(ptr, millisecs) {
        const string = UTF8ToString(ptr);
        const id = window.setTimeout(() => {
          const cString = stringToNewUTF8(string);
          try {
            _evalInSandbox(cString);
          } catch (_) {
          } finally {
            _free(cString);
            this.timeoutIds.delete(id);
          }
        }, millisecs);
        this.timeoutIds.add(id);
        return id;
      }

      clearTimeout(id) {
        if (this.timeoutIds.has(id)) {
          window.clearTimeout(id);
          this.timeoutIds.delete(id);
        }
      }

      setInterval(ptr, millisecs) {
        const string = UTF8ToString(ptr);
        const id = window.setInterval(() => {
          const cString = stringToNewUTF8(string);
          try {
            _evalInSandbox(cString);
          } catch (_) {
          } finally {
            _free(cString);
          }
        }, millisecs);
        this.timeoutIds.add(id);
        return id;
      }

      clearInterval(id) {
        if (this.timeoutIds.has(id)) {
          window.clearInterval(id);
          this.timeoutIds.delete(id);
        }
      }

      clearIds() {
        this.timeoutIds.forEach(id => window.clearTimeout(id));
        this.timeoutIds.clear();
      }
    }

    const inst = new Timeout();
    _setTimeout = inst.setTimeout.bind(inst);
    _clearTimeout = inst.clearTimeout.bind(inst);
    _setInterval = inst.setInterval.bind(inst);
    _clearInterval = inst.clearInterval.bind(inst);
    _clearIds = inst.clearIds.bind(inst);
  },
  setTimeout: function() {},
  setTimeout__deps: ["$timeout"],
  clearTimeout: function() {},
  clearTimeout__deps: ["$timeout"],
  setInterval: function() {},
  setInterval__deps: ["$timeout"],
  clearInterval: function() {},
  clearInterval__deps: ["$timeout"],
  clearIds: function() {},
  clearIds__deps: ["$timeout"],
  crackURL: function(ptr) {
    let result;
    try {
      const url = new window.URL(UTF8ToString(ptr));
      result = window.JSON.stringify({
        "cScheme": url.protocol,
        "cUser": url.username,
        "cPassword": url.password,
        "cHost": url.hostname,
        "cPort": url.port,
        "cPath": url.pathname,
        "cParameters": url.search,
        "cFragments": url.frag,
      });
    } catch (error) {
      result = error.message;
    }
    return stringToNewUTF8(result);
  },
  crackURL__deps: ["$stringToNewUTF8"],
  debugMe: function(ptr, alert) {
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
  printError: function(name_ptr, message_ptr, stack_ptr, alertOnError) {
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
  logMemUse: function(ptr) {
    const string = UTF8ToString(ptr);
    window.console.log(string);
  }
});
