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

const { execFileSync } = require("child_process");
const { readFileSync } = require("fs");

function git(dir, args) {
  return execFileSync("git", args, {
    cwd: dir,
    encoding: "utf8",
    stdio: ["ignore", "pipe", "ignore"],
  }).trim();
}

function getRepoRevision(dir, gitOptions = []) {
  try {
    const revision = git(dir, [...gitOptions, "rev-parse", "HEAD"]);
    const status = git(dir, [
      ...gitOptions,
      "status",
      "--porcelain",
      "--untracked-files=normal",
    ]);
    return status ? `${revision}-dirty` : revision;
  } catch {
    return "unknown";
  }
}

function getQuickjsRevision(dir) {
  // In Docker, HEAD is the commit created by applying our patch on top of the
  // upstream revision, so prefer the upstream one.
  if (process.env.QUICKJS_GIT_HASH) {
    return process.env.QUICKJS_GIT_HASH;
  }
  try {
    return git(dir, ["rev-parse", "HEAD"]);
  } catch {
    return "unknown";
  }
}

function readBuildMetadata(wasmFile) {
  const module = new WebAssembly.Module(readFileSync(wasmFile));
  const [section] = WebAssembly.Module.customSections(module, "build_info");
  return section ? JSON.parse(new TextDecoder().decode(section)) : undefined;
}

if (require.main === module) {
  const [quickjsDir, sourceDir, emscripten, flags] = process.argv.slice(2);
  const quickjs = getQuickjsRevision(quickjsDir);
  // The mounted checkout can belong to a different user inside Docker.
  const repo =
    process.env.REPO_REVISION ||
    getRepoRevision(sourceDir, ["-c", "safe.directory=*"]);
  process.stdout.write(JSON.stringify({ quickjs, repo, emscripten, flags }));
}

module.exports = { getRepoRevision, readBuildMetadata };
