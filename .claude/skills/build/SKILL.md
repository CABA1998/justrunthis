---
name: build
description: Compile the JustRunThis Qt app with CMake + Ninja and report compilation errors. Use after modifying any C++ source or CMakeLists.txt to verify the project still builds before claiming a task is complete.
allowed-tools: Bash, Read
---

# Build JustRunThis

Incremental build with Ninja. Reports success, warnings, or the first real compilation error.

## Decide: configure or just build?

Run the **build** step alone in the common case. Run the **configure** step first only when one of these is true:

- Any `CMakeLists.txt` was modified
- A new source file was added under `src/runners/` or `src/windows/`
- The `build/` directory does not exist

Editing code inside an existing `.cpp`/`.h` file does **not** require reconfigure.

## Build

From the project root:

```bash
cmake --build build
```

Output artifact:

- Windows: `build/justrunthis.exe`
- Linux:   `build/justrunthis`

## Configure (only when the rules above trigger it)

**Windows / Git Bash** (default for this repo):

```bash
export PATH="/c/Qt/Tools/mingw1310_64/bin:/c/Qt/Tools/Ninja:$PATH"
/c/Qt/Tools/CMake_64/bin/cmake \
    -S . -B build -G "Ninja" \
    -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64"
```

**Linux**:

```bash
cmake -S . -B build -G Ninja
```

Full setup (Qt install, PATH persistence, Qt Creator alternative) lives in [README.md](../../../README.md). Do not duplicate it here.

## Reporting results

- **Success**: exit code 0 and no `error:` lines. If there are `warning:` lines, mention them — they usually matter under `-Wall -Wextra`.
- **Compile error**: g++ prints `path:line:col: error: message`. Report the file, line, and the first error. Subsequent errors are often cascades from the first one.
- **Link error**: check if a new `.cpp` was added without being listed in its subdirectory `CMakeLists.txt`. That is the usual cause.
- **CMake can't find Qt**: stop and ask the user. Do **not** edit `CMAKE_PREFIX_PATH` or try to install Qt — these paths are machine-specific.

## Gotchas

- **Do not run the executable** after a successful build unless the user explicitly asked. This skill verifies that the code compiles, not that it behaves correctly.
- **Do not delete `build/`** to force a clean rebuild unless the user requests it. Ninja's incremental logic is reliable and full rebuilds take significantly longer.
- If `cmake --build build` fails with errors about missing build files or unknown targets, that means configure was never run — run the configure step, then retry.
- On Windows, the post-build `windeployqt` step copies Qt runtime DLLs next to the executable. If it fails but compilation succeeded, report it as a warning — the code is correct, only the ready-to-distribute bundle is incomplete.
- New `.cpp` files must be added to the appropriate `src/runners/CMakeLists.txt` or `src/windows/CMakeLists.txt` *and* the configure step must be re-run. Just adding the file is not enough.
