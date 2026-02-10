# Building

This repo uses CMake presets and SDL3. Please ensure you have SDL3 installed, it is not included in `third_party`.

## One-Command Build Scripts

- Linux: `./build-linux` or `./build-linux release`
- macOS: `./build-macos` or `./build-macos release`
- Windows: `build-windows.bat` or `build-windows.bat release`

## Using Presets Directly

Configure + build:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
```

Run:

```bash
cmake --build --preset linux-debug --target run
```

## No-Presets (Manual)

```bash
cmake -S . -B build/local -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/local
./build/local/Game
```
