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

const { ArgumentParser } = require("argparse");
const fs = require("fs");
const { spawn, exec } = require("child_process");
const { resolve } = require("path");

function execAndPrint(fun, args) {
  const child = spawn(fun, args.split(" "), {stdio: "inherit"});
  return new Promise((resolve) => {
    child.on("close", () => {
      resolve();
    });
  });
}

function create() {
  return execAndPrint("docker", "build -t qjs-sandbox .");
}

function build(path) {
  const workingDir = resolve(".");
  const uid = process.getuid();
  const gid = process.getgid();
  return execAndPrint("docker", `run -t -v ${path}:/js -v ${workingDir}:/code --rm --user ${uid}:${gid} qjs-sandbox`);
}

function compile(path) {
  path = resolve(path);
  fs.access(path, fs.constants.F_OK, (err) => {
    if (err) {
      console.log(`Invalid path: ${path}`);
      return;
    }
    exec("docker images qjs-sandbox", (err, stdout) => {
      const output = stdout.split("\n").map(line => line.trim()).filter(line => !!line);
      if (output.length === 1) {
        create().then(() => {
          build(path);
        });
      } else {
        build(path);
      }
    });
  })
}

const parser = new ArgumentParser({
  description: "Build sandbox"
});

parser.add_argument(
  "-C",
  "--create",
  {
    help: "Create the docker image",
    action: "store_true",
  }
);
parser.add_argument(
  "-c",
  "--compile",
  {
    help: "Compile the sandbox and output a js file",
    action: "store_true",
  }
);
parser.add_argument(
  "-o",
  "--output",
  {
    help: "Output directory",
    default: ".",
  }
);

const args = parser.parse_args()
if (args.create) {
  create()
}
if (args.compile) {
  compile(args.output)
}
