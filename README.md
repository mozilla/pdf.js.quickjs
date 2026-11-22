# pdf.js.quickjs

Provide a sandbox to PDF.js.

## Build

Run:

```sh
node build.js --compile --output my_output_dir
```

This builds the Docker image using Docker's layer cache, then writes `quickjs-eval.js` and `quickjs-eval.wasm` to `my_output_dir`.

### Build metadata

`quickjs-eval.wasm` contains a `build_info` custom section with a JSON object describing the build: the QuickJS revision (`quickjs`), the revision of this repository (`repo`, suffixed with `-dirty` if there were local changes), the Emscripten version (`emscripten`) and the compiler flags (`flags`). It can be read with:

```js
const module = await WebAssembly.compile(bytes);
const [section] = WebAssembly.Module.customSections(module, "build_info");
const buildInfo = JSON.parse(new TextDecoder().decode(section));
```

## Update

To update QuickJS, change the commit hash in `Dockerfile` and run:

```sh
node build.js --compile --output my_output_dir
```

Use `node build.js --create` to build only the Docker image.

## Release

Pushing a version tag (e.g. `v1.2.3`) runs the `Release` workflow: it builds the sandbox and opens a pull request in [pdf.js](https://github.com/mozilla/pdf.js) updating `external/quickjs/`, titled "Update QuickJS sandbox to version 1.2.3":

```sh
git tag v1.2.3
git push origin v1.2.3
```

The workflow refuses to replace a sandbox in pdf.js that was built from a revision which isn't an ancestor of the tagged one, so that retrying an old tag can't revert a newer release.

The workflow runs in the `release` environment, which must provide the `CLIENT_ID` and `APP_PRIVATE_KEY` secrets of a GitHub App installed on `mozilla/pdf.js` with read and write access to contents and pull requests.

## Licensing

The code is released under [MIT license](https://opensource.org/licenses/MIT).
