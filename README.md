# JustRunThis 

Local runner for competitive-programming C++ snippets. A Qt Widgets desktop
app that lets you pick a `.cpp` file from a folder tree, paste stdin, and
click one button to compile and run it — no internet required.

![layout](https://img.shields.io/badge/ui-Qt%20Widgets-41cd52) ![lang](https://img.shields.io/badge/lang-C%2B%2B17-blue) ![license](https://img.shields.io/badge/license-MIT-lightgrey)

---

## Features (MVP)

- Folder tree (left panel) — open any folder as a workspace.
- stdin editor (top-right) — paste sample input from Codeforces.
- stdout/stderr view (bottom-right) — shows compiler errors and program
  output with a monospaced font.
- One-click **Run** — invokes `g++ -std=c++17 -O2 -Wall`, then executes.
- 5-second execution timeout (protection against infinite loops).
- Elapsed time reported in the status label.

---

## Build and run on Windows

### 1. Prerequisites — Qt + MinGW

Install Qt 6 via the **Qt Online Installer**:

- Download: https://www.qt.io/download-qt-installer-oss
- Create a free Qt account if you don't have one.
- In the "Select Components" screen, enable:
  - `Qt → Qt 6.11.0 → MinGW 13.1.0 64-bit`
  - `Developer and Designer Tools → MinGW 13.1.0 64-bit`
  - `Developer and Designer Tools → CMake`
  - `Developer and Designer Tools → Ninja`

After installation you will have:

- `C:\Qt\6.11.0\mingw_64\` — Qt framework (headers + libs + runtime DLLs)
- `C:\Qt\Tools\mingw1310_64\` — MinGW compiler (g++, ld)
- `C:\Qt\Tools\CMake_64\bin\cmake.exe` — CMake
- `C:\Qt\Tools\Ninja\ninja.exe` — Ninja build system

> Adjust the version numbers (`6.11.0`, `1310`) if yours differ. The steps
> below assume the paths above.

### 2. Configure (one-time)

Open **Git Bash** in the project root and run:

```bash
export PATH="/c/Qt/Tools/mingw1310_64/bin:/c/Qt/Tools/Ninja:$PATH"

/c/Qt/Tools/CMake_64/bin/cmake \
    -S . -B build -G "Ninja" \
    -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64"
```

Or in **PowerShell**:

```powershell
$env:Path = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;" + $env:Path

& "C:\Qt\Tools\CMake_64\bin\cmake.exe" `
    -S . -B build -G "Ninja" `
    -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64"
```

You only need to re-run this when you change `CMakeLists.txt` or add new
source files.

### 3. Build

**Git Bash:**

```bash
/c/Qt/Tools/CMake_64/bin/cmake --build build
```

**PowerShell:**

```powershell
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" --build build
```

Run this every time you change the code. The output is `build/justrunthis.exe`.

### 4. Run

The app needs Qt's runtime DLLs on `PATH`.

**Git Bash:**

```bash
export PATH="/c/Qt/6.11.0/mingw_64/bin:$PATH"
./build/justrunthis.exe
```

**PowerShell:**

```powershell
$env:Path = "C:\Qt\6.11.0\mingw_64\bin;" + $env:Path
.\build\justrunthis.exe
```

### 5. (Optional) Persist the PATH so you don't have to set it every time

Add these directories to your permanent user `PATH` via
`Settings → System → About → Advanced system settings → Environment Variables`:

- `C:\Qt\Tools\mingw1310_64\bin`
- `C:\Qt\6.11.0\mingw_64\bin`

After that, double-clicking `build\justrunthis.exe` from File Explorer will
also work.

### Alternative — build with Qt Creator

1. Launch Qt Creator.
2. `File → Open File or Project…` → choose this project's `CMakeLists.txt`.
3. Pick the **Desktop Qt 6.11.0 MinGW 64-bit** kit when prompted.
4. Press **Ctrl+R** to build and run. Qt Creator sets the `PATH` for you.

---

## Build and run on Ubuntu / Debian

### 1. Install dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    qt6-base-dev \
    qt6-tools-dev
```

> On Ubuntu 22.04 this gives you Qt 6.2. On Ubuntu 24.04 you get Qt 6.4.
> Any Qt 6.x works — our `CMakeLists.txt` requires only `Qt6 Widgets`.

If `qt6-base-dev` is not available on your distro (very old Ubuntu), use
the Qt Online Installer instead — same URL as the Windows section, pick
the Linux components.

### 2. Configure

```bash
cmake -S . -B build -G Ninja
```

If CMake cannot find Qt (unusual on Debian/Ubuntu packages), point it
explicitly:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```

### 3. Build

```bash
cmake --build build
```

Output: `build/justrunthis`.

### 4. Run

```bash
./build/justrunthis
```

No `PATH` setup needed — Qt shared libraries are installed in the system
library path by apt.

---

## Using the app

1. **`File → Open Folder`** and pick the folder with your C++ solutions.
2. Navigate the left tree, click on a `.cpp` file. The **Run** button
   becomes enabled.
3. Paste sample input in the top-right **Input (stdin)** panel (optional).
4. Click **Run**. Status goes `Compiling…` → `Running…` → `Done in X ms`.
5. Program output (stdout + stderr) appears in the bottom **Output** panel.
6. If a program runs longer than 5 seconds it is killed automatically
   (status: `Timeout — killed after 5000 ms`).

Compiled binaries are cached in:

- Windows: `%LOCALAPPDATA%\justrunthis\justrunthis\cache`
- Linux:   `~/.cache/justrunthis/`

They are not written next to your source files.

---

## Project layout

```
justrunthis/
├── CMakeLists.txt          # Build config (Qt6 Widgets, C++17)
├── src/
│   ├── main.cpp            # QApplication bootstrap
│   ├── MainWindow.{h,cpp}  # UI layout, menu, signals
│   └── CppRunner.{h,cpp}   # g++ subprocess + run subprocess + timeout
├── samples/
│   └── sum.cpp             # Example input/output for smoke testing
└── README.md
```

---

## License

MIT — see [LICENSE](LICENSE).
