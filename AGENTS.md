# Repository Agent Instructions

## Project
Grok Desktop: minimal Windows x64 C++20 + CEF application opening `https://grok.com/`.

## Rules
- Read `README.md`, `AGENT_PROMPT.md`, and `docs/` before implementation.
- Use CEF only. Do not switch to Electron, Node.js, WebView2, or another web engine.
- Preserve existing user data and inspect before overwriting.
- Use Visual Studio 2022/MSVC and CMake; build Release x64.
- Use official CEF binary distributions and verify hashes when published.
- Never invent dependency versions, APIs, download URLs, test results, or output paths.
- Never log passwords, cookies, tokens, or page contents.
- Never disable TLS certificate validation or bypass Grok authentication/security.
- Do not include personal profile/cache files in distributable packages.
- Run actual build and tests; report untested behavior honestly.
- Keep the first version minimal; no backend, analytics, auto-updater, or generic browser features.
