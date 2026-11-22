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

const assert = require("node:assert/strict");
const { execFileSync } = require("node:child_process");
const { mkdtempSync, rmSync, writeFileSync } = require("node:fs");
const { tmpdir } = require("node:os");
const { join } = require("node:path");
const { test } = require("node:test");

test("build metadata records the revision and local changes", (t) => {
  const dir = mkdtempSync(join(tmpdir(), "quickjs-metadata-"));
  t.after(() => rmSync(dir, { recursive: true, force: true }));

  const env = { ...process.env, REPO_REVISION: "" };
  delete env.GIT_INDEX_FILE;
  delete env.QUICKJS_GIT_HASH;
  const options = { cwd: dir, encoding: "utf8", env };
  const git = (...args) =>
    execFileSync(
      "git",
      ["-c", "user.name=Test", "-c", "user.email=test@example.com", ...args],
      options,
    ).trim();

  const source = join(dir, "exports.c");
  writeFileSync(source, "original\n");
  git("init", "--quiet");
  git("add", ".");
  git("commit", "--quiet", "--no-gpg-sign", "-m", "Initial commit");
  git("tag", "--annotate", "--no-sign", "-m", "Build tag", "build-tag");

  const revision = git("rev-parse", "HEAD");
  const emscripten = "1.2.3";
  const flags = '-s EXPORTED_FUNCTIONS=["_malloc", "_free"]\t-O3';
  const args = [
    require.resolve("../build_metadata.js"),
    dir,
    dir,
    emscripten,
    flags,
  ];
  const readMetadata = (extraEnv = {}) =>
    JSON.parse(
      execFileSync(process.execPath, args, {
        ...options,
        env: { ...env, ...extraEnv },
      }),
    );

  assert.deepEqual(readMetadata(), {
    quickjs: revision,
    repo: revision,
    emscripten,
    flags,
  });

  // The upstream revision is preferred to the patched checkout one.
  const upstream = "0123456789abcdef0123456789abcdef01234567";
  assert.equal(readMetadata({ QUICKJS_GIT_HASH: upstream }).quickjs, upstream);

  git("config", "status.showUntrackedFiles", "no");
  const untracked = join(dir, "untracked.h");
  writeFileSync(untracked, "untracked\n");
  assert.equal(readMetadata().repo, `${revision}-dirty`);
  rmSync(untracked);
  assert.equal(readMetadata().repo, revision);

  writeFileSync(source, "modified\n");
  assert.equal(readMetadata().repo, `${revision}-dirty`);

  writeFileSync(join(dir, ".git", "index"), "invalid index\n");
  assert.equal(readMetadata().repo, "unknown");
});
