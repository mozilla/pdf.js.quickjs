"use strict";

const js = require("@eslint/js");
const globals = require("globals");
const prettier = require("eslint-config-prettier");

module.exports = [
  { ignores: ["node_modules/", "**/quickjs-eval.js"] },
  js.configs.recommended,
  prettier,
  {
    // Build scripts, tests and config: plain Node CommonJS.
    files: [
      "build.js",
      "build_metadata.js",
      "eslint.config.js",
      "test/**/*.js",
    ],
    languageOptions: {
      ecmaVersion: 2022,
      sourceType: "commonjs",
      globals: { ...globals.node },
    },
  },
  {
    // Emscripten --js-library file: spliced into the generated module, so it
    // runs with the Emscripten runtime and browser globals in scope.
    files: ["src/myjs.js"],
    languageOptions: {
      ecmaVersion: 2022,
      sourceType: "script",
      globals: {
        ...globals.browser,
        mergeInto: "readonly",
        LibraryManager: "readonly",
        Module: "readonly",
        UTF8ToString: "readonly",
        stringToNewUTF8: "readonly",
        _callExternalFunction: "writable",
      },
    },
  },
];
