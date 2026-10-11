# Building Grok Desktop

Grok Desktop is a minimal Windows C++20 application built on the Chromium
Embedded Framework (CEF). It opens `https://grok.com/` in a native Win32
window with a small navigation toolbar.

## Prerequisites

| Requirement | Notes |
|---|---|
| Windows 10/11 x64 | Build and target platform. |
| Visual Studio 2022 | Desktop development with C++ workload; MSVC v143; Windows 10/11 SDK (10.0.22621+). |
| CMake 3.21+ | Ships with VS 2022 or install separately. |
| CEF binary distribution | Official `cef_binary_*_windows64.tar.bz2`, extracted to `third_party/cef/`. |
| 7-Zip (optional) | Used by `package.ps1` for ZIP creation; PowerShell `Compress-Archive` is the fallback. |
| Inno Setup 6 (optional) | Used to compile the installer. |

Run the environment check first:

```powershell
.\scripts\check-environment.ps1
```

## CEF setup

1. Download the official CEF binary distribution from
   <https://cef-builds.spotifycdn.com/index.json> (stable channel).
2. Verify the archive hash against the SHA-1 published in `index.json`
   (and record the SHA-256 for your own manifest).
3. Extract to `third_party/cef/` so that `third_party/cef/CMakeLists.txt`
   exists.

The project has been developed and tested against:

```
cef_binary_154.0.34+g14c5a08+chromium-154.0.8037.98_windows64.tar.bz2
SHA-1 (official): 8c705a2ef1ce5f7b9655729a2d3de5d033a24d8b
SHA-256 (local):  51ebf99f026c5901bb1c647053a00dc4abed70fe24f5d9bcfaaa0016b3f774b
```

## Build (script)

```powershell
# Release x64, CEF sandbox ON (default)
.\scripts\build.ps1

# Clean rebuild
.\scripts\build.ps1 -Clean

# Without the CEF sandbox (uses a plain WIN32 executable instead of
# bootstrap.exe + DLL)
.\scripts\build.ps1 -NoSandbox
```

Output: `build\src\Release\GrokDesktop.exe` (or `build-nosandbox\...`).

## Build (manual CMake)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DUSE_SANDBOX:BOOL=ON
cmake --build build --config Release -- /m
```

CMake presets are also available:

```powershell
cmake --preset vs2022-x64-sandbox
cmake --build --preset release-sandbox
```

## Sandbox vs no-sandbox

| `USE_SANDBOX` | Target type | Launcher | Notes |
|---|---|---|---|
| `ON` (default) | Shared DLL (`GrokDesktop.dll`) | `bootstrap.exe` renamed to `GrokDesktop.exe` | Matches the official CEF Windows sandbox distribution (no `cef_sandbox.lib` shipped; uses the bootstrap loader). LPAC ACLs are applied post-build. |
| `OFF` | Win32 executable | `GrokDesktop.exe` | Links normally; a Windows manifest is embedded because CEF's shared linker flags disable the default manifest. |

Both modes are supported by the same source code; the entry point switches
between `RunWinMain` (sandbox bootstrap) and `wWinMain` via the
`CEF_USE_BOOTSTRAP` compile definition.

## Package

```powershell
# Portable ZIP + Inno Setup installer + SHA256SUMS.txt
.\scripts\package.ps1

# Portable ZIP only
.\scripts\package.ps1 -PortableOnly

# Installer only
.\scripts\package.ps1 -InstallerOnly
```

Outputs:

- `dist/portable/GrokDesktop/` — unpacked portable layout
- `dist/GrokDesktop-<version>-win64-portable.zip`
- `dist/installer/GrokDesktop-<version>-win64-setup.exe`
- `dist/SHA256SUMS.txt`

## Runtime profile location

User data (cookies, cache, login state) is stored in:

```
%LOCALAPPDATA%\GrokDesktop\CEFProfile
```

The portable build does **not** store data next to the executable, so the
portable folder can be moved or deleted without losing the Grok login.

## What the app does (and does not do)

- Opens `https://grok.com/` in a native Win32 window hosting a CEF browser
  child window.
- Toolbar: Back, Forward, Reload, Home (DPI-aware).
- Single instance: launching a second copy activates the existing window.
- Persistent profile under `%LOCALAPPDATA%\GrokDesktop`.
- Popups (`window.open`, OAuth flows) are embedded as child windows of the
  main window so the app remains a single native window.
- Only `https:`, `http:`, `about:` and `data:` URLs are allowed; anything
  else is blocked in `OnBeforeBrowse`, `OnOpenURLFromTab` and
  `OnBeforePopup`.
- TLS certificate errors are never bypassed.
- No passwords, cookies, tokens or page contents are logged.

Not included (intentionally): backend services, analytics, auto-updater,
generic browser features (tabs, bookmarks, extensions, DevTools UI).
