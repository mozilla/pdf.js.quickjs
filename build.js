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

"use strict";

const { ArgumentParser } = require("argparse");
const fs = require("fs");
const { spawn } = require("child_process");
const { resolve } = require("path");

function execAndPrint(fun, args) {
  const child = spawn(fun, args, { stdio: "inherit" });
  return new Promise((resolve, reject) => {
    child.on("error", reject);
    child.on("close", (code) => {
      if (code === 0) {
        resolve();
        return;
      }
      reject(new Error(`${fun} exited with code ${code}`));
    });
  });
}

function create() {
  return execAndPrint("docker", ["build", "-t", "qjs-sandbox", "."]);
}

function build(path) {
  const workingDir = resolve(".");
  const args = [
    "run",
    "-t",
    "-v",
    `${path}:/js`,
    "-v",
    `${workingDir}:/code`,
    "--rm",
  ];
  // process.getuid/getgid aren't available on Windows. They're used to make
  // the generated files owned by the current user instead of root, which is
  // only a concern on POSIX platforms.
  if (process.getuid && process.getgid) {
    args.push("--user", `${process.getuid()}:${process.getgid()}`);
  }
  args.push("qjs-sandbox");
  return execAndPrint("docker", args);
}

async function hasImage() {
  try {
    await execAndPrint("docker", ["image", "inspect", "qjs-sandbox"]);
    return true;
  } catch {
    return false;
  }
}

async function compile(path) {
  path = resolve(path);
  await fs.promises.access(path, fs.constants.F_OK);
  if (!(await hasImage())) {
    await create();
  }
  await build(path);
}

const parser = new ArgumentParser({
  description: "Build sandbox",
});

parser.add_argument("-C", "--create", {
  help: "Create the docker image",
  action: "store_true",
});
parser.add_argument("-c", "--compile", {
  help: "Compile the sandbox and output a js file",
  action: "store_true",
});
parser.add_argument("-o", "--output", {
  help: "Output directory",
  default: ".",
});

const args = parser.parse_args();

async function main() {
  if (args.create) {
    await create();
  }
  if (args.compile) {
    await compile(args.output);
  }
}

main().catch((error) => {
  console.error(error.message);
  process.exitCode = 1;
});
