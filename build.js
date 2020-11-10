/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
