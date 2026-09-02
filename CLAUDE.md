# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

KMS-Tools is a suite of small, standalone Windows utilities (Com port tool, Modbus tool/simulator, WOP tool, lab-instrument controller, EOL tester, a geometry helper, and a launcher) built on top of **KMS-Framework**, a separate C++ library product. Most components are C++ CLI tools; one (`Launcher`) also serves a React front end so tools can be driven from a browser; `USB-To` is an independent Python utility unrelated to the C++/KMS-Framework stack.

## Dependencies / getting a working tree

- `Import/` (prebuilt KMS-Framework headers, static libs, and the `KMS-Build`/`KMS-Import`/`KMS-Make`/`KMS-Sync` tool binaries) is **git-ignored** and not checked in. It is fetched by running the `KMS-Import` tool against `KMS-Import.cfg`, which pins the framework version (`Dependencies += KMS-Framework;<version>`). Without a populated `Import/`, the C++ projects will not compile.
- Every `<KMS/...>` header used across the codebase (e.g. `KMS/Main.h`, `KMS/Banner.h`, `KMS/CLI/Tool.h`) comes from `Import/Includes`; the corresponding static libs are `Import/Libraries/<Config>_<Processor>/KMS-A.a` / `KMS-B.a` (referenced from `Common.mk` for the Linux/Makefile side).
- `front-end/` is a standard Create React App project (`npm install` then the scripts in `package.json`).
- `USB-To/` only needs `pip install -r USB-To/requirements.txt` (currently just `hidapi`).

## Building

- C++ components are Visual Studio projects under `Solution.sln` (toolset v143, Windows target 10.0). Build via Visual Studio or `MSBuild Solution.sln`. Configurations: `Debug`, `Release`, `Release_Static` (Windows-only) across `x64`/`x86`. Most binaries (ComTool, EOL, LabCtrl, ModbusSim, ModbusTool, WOP-Tool) are Windows-only; `Launcher` and `PGeo` also build on Linux (see `Linux*` vs `Windows*` prefixes in `KMS-Build.cfg`).
- `front-end`: `npm run build` (produces `front-end/build`, which `Launcher` serves).
- There is no CI config in this repo (no `.github/workflows`); building/testing is manual.

## Packaging / release (KMS-Build)

Packaging, version stamping, and changelog maintenance are driven by the imported `KMS-Build.exe` tool reading `KMS-Build.cfg` files (one at the repo root, one under `front-end/`), **not** by hand-editing versions everywhere:

- The canonical version lives in `Common/Version.h` (`VERSION_RC`, `VERSION_STR`, `KMS_VERSION("dev"|"rc"|...)`), referenced by `VersionFile` in the root `KMS-Build.cfg`.
- Each `KMS-Build.cfg` lists `Binaries`/`WindowsBinaries` to build, `Files`/`WindowsFiles` to stamp/package, and `EditOperations` — regex substitutions applied to specific files on build (e.g. bumping `AppVersion=` in `Product_x64.iss`, or the `VERSION` const in `front-end/src/pages/Layout.js`).
- Every packaged component has a `<Component>/_DocUser/KMS-Tools.<Component>.ReadMe.txt` with a fixed header (Author/Copyright/License/Product/File) followed by an `EDIT ON BUILD` marker; `KMS-Build` appends a new dated version entry under that marker on each release. The root `_DocUser/KMS-Tools.ReadMe.txt` aggregates which components changed per release. When adding a new packaged file/component, register it in `KMS-Build.cfg`'s `Files`/`WindowsFiles` list or `KMS-Build` won't stamp or ship it.
- `_DocDev/Build.txt` is the manual release checklist (bump `Common/Version.h`, search for `EDIT ON BUILD`, run `KMS-Build.exe`, commit/push, optionally upload to GitHub).
- `Product_x64.iss` is the Inno Setup installer script; it explicitly lists every file/exe to bundle, so new components/binaries must be added there too (see how `USB-To/USB-To-I2C.py` and `USB-To/requirements.txt` are listed even though USB-To isn't wired into `KMS-Build.cfg`).

## Architecture patterns shared by the C++ tools

Every C++ tool follows the same skeleton:

```cpp
#include <KMS/Base.h>
#include <KMS/Banner.h>
#include <KMS/Cfg/MetaData.h>
#include <KMS/Main.h>
#include "../Common/Version.h"
```

- Entry point goes through `KMS::Main`; configuration objects derive from KMS-Framework's `DI` (data/introspection) types and are described via `KMS::Cfg::MetaData`, which is what enables the uniform `Key=Value` / `Key+=Value` command-line and config-file argument style used everywhere (e.g. `Launcher.exe Exec=ComTool.exe Page=ComTool Routes+=/ComTool Title=KMS-Tools`).
- Interactive tools (ComTool, LabCtrl, ModbusTool, WOP-Tool) subclass `KMS::CLI::Tool` and use `KMS::CLI::Macros`/`KMS::CLI::CommandLine`, giving them a scriptable REPL. There is no separate unit-test framework in this repo — "tests" are macro scripts run through that same REPL (see `LabCtrl/Tests/*.txt`, which use `Echo`/`AbortIfError`/`ClearError`/`Exit` and end with `Echo Test PASSED` / `Exit 0`).
- LabCtrl in particular is organized around an `IInstrument` interface (`LabCtrl/IInstrument.h`) implementing `KMS::DAQ::{AnalogInput,AnalogOutput,DigitalInput,DigitalOutput}`, with per-vendor instrument implementations under `LabCtrl/Siglent/` and a generic `LabCtrl/SCPI.*` layer.

## Launcher + front-end (GUI mode)

Each CLI tool can be run headless or through a browser GUI:

- `<Component>/<Component>_GUI.cmd` invokes `Launcher.exe Exec=<Tool>.exe Page=<Page> Routes+=/<Page> Title=KMS-Tools`.
- `Launcher` (via `KMS::HTTP::HTTP`/`KMS::HTTP::ReactApp`/`KMS::HTTP::Transaction` and `KMS::Proc::Process`/`KMS::Proc::Browser`) serves the built React app from `front-end/build`, opens a browser, and proxies/execs the target tool as a child process for the requested route.
- Routes map 1:1 to pages in `front-end/src/pages/` (`ComTool.js`, `ModbusSim.js`, `ModbusTool.js`, `Launcher.js`, `WOP-Tool.js`, `Home.js`), which share form-building components in `front-end/src/components/` (`KMSArguments.js`, `LauncherParams.js`, `Enum.js`, `Flag.js`, `Numeric.js`, `Text.js`, `TextList.js`, `File.js`, `FileList.js`, `Folder.js`). Adding GUI support for a new tool means adding both a `<Tool>_GUI.cmd` and a matching front-end page.

## USB-To

`USB-To/USB-To-I2C.py` is a self-contained Python 3 script (no KMS-Framework dependency) that talks directly to the WaveShare USB-TO-UART/I2C/SPI/JTAG board's CH347 chip over its HID I2C-stream protocol, using `hidapi` — no vendor SDK. Run it directly (`python USB-To/USB-To-I2C.py`) after installing `USB-To/requirements.txt`.
