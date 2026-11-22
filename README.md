# pdf.js.quickjs

Provide a sandbox to PDF.js.

## Build

Run:

```sh
node build.js --compile --output my_output_dir
```

it will create a Docker image with emsdk and then run it. The generated `quickjs-eval.js` will be in `my_output_dir`.

### Build metadata

`quickjs-eval.wasm` contains a `build_info` custom section with a JSON object describing the build: the QuickJS revision (`quickjs`), the revision of this repository (`repo`, suffixed with `-dirty` if there were local changes), the Emscripten version (`emscripten`) and the compiler flags (`flags`). It can be read with:

```js
const module = await WebAssembly.compile(bytes);
const [section] = WebAssembly.Module.customSections(module, "build_info");
const buildInfo = JSON.parse(new TextDecoder().decode(section));
```

## Update

In order to update quickjs to a specific revision, change the commit hash in `Dockerfile` and then run:

```sh
node build.js --create
```

to create a new docker image and then

```sh
node build.js --compile --output my_output_dir
```

to compile. The short version is:

```sh
node build.js -Cco my_output_dir
```

## Release

Pushing a version tag (e.g. `v1.2.3`) runs the `Release` workflow: it builds the sandbox and opens a pull request in [pdf.js](https://github.com/mozilla/pdf.js) updating `external/quickjs/`, titled "Update QuickJS sandbox to version 1.2.3":

```sh
git tag v1.2.3
git push origin v1.2.3
```

The workflow runs in the `release` environment, which must provide the `CLIENT_ID` and `APP_PRIVATE_KEY` secrets of a GitHub App installed on `mozilla/pdf.js` with read and write access to contents and pull requests.

## Licensing

The code is released under [MIT license](https://opensource.org/licenses/MIT).
