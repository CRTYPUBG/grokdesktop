// Grok Desktop entry point: CEF multi-process bootstrap, settings and message
// loop. The same binary serves as the browser process and as the sub-process
// (renderer, GPU, etc.) executable, following the standard CEF pattern.
#include <windows.h>

#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "include/cef_version_info.h"
#include "resources/resource.h"
#include "src/App.h"
#include "src/app_config.h"

namespace {

int RunMain(HINSTANCE hinstance, int /*nCmdShow*/, void* sandbox_info) {
  CefMainArgs main_args(hinstance);

  // When this executable is started as a CEF sub-process this returns the
  // sub-process exit code; otherwise it returns -1 and the browser process
  // continues below.
  const int exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
  if (exit_code >= 0) {
    return exit_code;
  }

  CefRefPtr<App> app(new App(hinstance));
  if (!app->InitializeProfilePaths()) {
    MessageBoxW(nullptr,
                L"Uygulama profil dizini olusturulamadi.\n"
                L"Klasor izinlerini kontrol edin ve tekrar deneyin.",
                kAppName, MB_OK | MB_ICONERROR);
    return 1;
  }

  CefSettings settings;
  if (!sandbox_info) {
    settings.no_sandbox = true;
  }

  // Persistent profile. cache_path must be equal to, or a child directory of,
  // root_cache_path for this CEF version.
  CefString(&settings.root_cache_path) = app->root_cache_path();
  CefString(&settings.cache_path) = app->cache_path();
  // Persist session cookies so a normal Grok sign-in survives restarts.
  settings.persist_session_cookies = 1;

  // Never write a log file next to the user profile; only fatal/error output
  // goes to the debug output stream.
  settings.log_severity = LOGSEVERITY_ERROR;

#if defined(CEF_USE_BOOTSTRAP)
  // The bootstrap.exe shim is the real executable in sandbox mode, so the
  // default Chrome windows (DevTools, Task Manager) take their icon from it.
  settings.chrome_app_icon_id = 32512;  // IDI_APPLICATION
#else
  settings.chrome_app_icon_id = IDI_GROKDESKTOP;
#endif

  // Early exit is possible when another instance already owns this profile
  // (process singleton) or when initialization fails.
  if (!CefInitialize(main_args, settings, app.get(), sandbox_info)) {
    return CefGetExitCode();
  }

  int result = 0;
  if (app->startup_failed()) {
    // Window or browser creation failed; the user already saw an error.
    result = 1;
  } else {
    // Blocks until CefQuitMessageLoop() is called after the last browser
    // window closes.
    CefRunMessageLoop();
  }

  CefShutdown();
  return result;
}

}  // namespace

#if defined(CEF_USE_BOOTSTRAP)

// Entry point called by bootstrap.exe when this target is built as a DLL.
CEF_BOOTSTRAP_EXPORT int RunWinMain(HINSTANCE hinstance,
                                    LPWSTR lp_cmd_line,
                                    int n_cmd_show,
                                    void* sandbox_info,
                                    cef_version_info_t* version_info) {
  UNREFERENCED_PARAMETER(lp_cmd_line);
  UNREFERENCED_PARAMETER(version_info);
  return RunMain(hinstance, n_cmd_show, sandbox_info);
}

#else  // !defined(CEF_USE_BOOTSTRAP)

int APIENTRY wWinMain(HINSTANCE hinstance,
                      HINSTANCE h_prev_instance,
                      LPWSTR lp_cmd_line,
                      int n_cmd_show) {
  UNREFERENCED_PARAMETER(h_prev_instance);
  UNREFERENCED_PARAMETER(lp_cmd_line);

  void* sandbox_info = nullptr;
#if defined(CEF_USE_SANDBOX)
  // Manage the life span of the sandbox information object. See
  // include/cef_sandbox_win.h for details.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  return RunMain(hinstance, n_cmd_show, sandbox_info);
}

#endif  // !defined(CEF_USE_BOOTSTRAP)
