# AGENT.md — JustRunThis

Qt Widgets desktop app (Windows + Linux) that compiles and runs `.cpp` files with a single click. Targets competitive programmers who need a local runner without an internet connection.

**Build instructions** → [README.md](README.md)  
**Branch strategy, code style, PR workflow** → [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md)

---

## Project layout

```
justrunthis/
├── CMakeLists.txt              # Root: C++17, Qt6 Widgets, AUTOMOC/AUTOUIC/AUTORCC
├── app/
│   ├── CMakeLists.txt          # Executable target + windeployqt post-build step
│   └── main.cpp                # QApplication bootstrap
├── src/
│   ├── runners/
│   │   ├── CppRunner.h / .cpp  # Compilation + execution engine
│   │   └── CMakeLists.txt      # Static lib, links Qt6::Core
│   └── windows/
│       ├── MainWindow.h / .cpp # UI orchestration
│       └── CMakeLists.txt      # Static lib, links Qt6::Widgets + runners
├── samples/
│   ├── sum.cpp                 # Batch stdin smoke test
│   └── hello.cpp               # Interactive stdin smoke test
└── docs/
    └── CONTRIBUTING.md
```

> The layout in README.md is outdated — the authoritative structure is above.

---

## Architecture

Two classes carry all the logic:

### `CppRunner` (`src/runners/`)

Manages two sequential `QProcess` objects — compiler then binary — and owns all subprocess state.

| Method | What it does |
|---|---|
| `run(sourceFile, stdinData, interactive)` | Entry point. Hashes source path for binary cache name, calls `startCompile()`. |
| `startCompile()` | Spawns `g++ -std=c++17 -O2 -Wall`. On success calls `startRun()`. |
| `startRun()` | Spawns compiled binary. Non-interactive: writes stdin buffer and arms a 5 s kill timer. |
| `sendLine(text)` | Writes a line to the running process stdin (interactive mode only). |
| `stop()` | Kills whichever process is currently alive. |

Signals emitted: `statusChanged(QString)`, `stdoutReceived(QString)`, `stderrReceived(QString)`, `finished(bool, int, qint64)`.

Binary cache:
- Windows: `%LOCALAPPDATA%\justrunthis\justrunthis\cache\`
- Linux: `~/.cache/justrunthis/`

### `MainWindow` (`src/windows/`)

Owns the UI and wires `CppRunner` signals to widgets. No business logic lives here.

Key widgets: `m_tree` (file browser), `m_input` (batch stdin), `m_console` + `m_consoleInput` (interactive mode), `m_stdout` / `m_stderr` (output), `m_modeCombo` (TXT vs Interactive), `m_runButton`, `m_status`.

Mode switching (`m_modeCombo` index 0 = TXT, 1 = Interactive) is handled by `onModeChanged()`, which swaps a `QStackedWidget` page and toggles stdout pane visibility.

---

## Testing

There are no automated tests. To verify a change, build and exercise the app manually using the files in `samples/`.
