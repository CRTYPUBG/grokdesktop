// CefApp implementation: browser-process lifecycle, profile paths and
// single-instance activation.
#ifndef GROKDESKTOP_SRC_APP_H_
#define GROKDESKTOP_SRC_APP_H_

#include <windows.h>

#include <memory>
#include <string>

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"

class BrowserClient;
class MainWindow;

class App : public CefApp, public CefBrowserProcessHandler {
 public:
  explicit App(HINSTANCE hinstance);

  App(const App&) = delete;
  App& operator=(const App&) = delete;

  // CefApp methods.
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  // CefBrowserProcessHandler methods.
  void OnContextInitialized() override;
  bool OnAlreadyRunningAppRelaunch(
      CefRefPtr<CefCommandLine> command_line,
      const CefString& current_directory) override;
  CefRefPtr<CefClient> GetDefaultClient() override;

  // Resolves and, if needed, creates the per-user profile directories.
  // Returns false if the directories cannot be created.
  bool InitializeProfilePaths();

  // True when window or browser creation failed during startup.
  bool startup_failed() const { return startup_failed_; }

  const std::wstring& root_cache_path() const { return root_cache_path_; }
  const std::wstring& cache_path() const { return cache_path_; }

 private:
  HINSTANCE const hinstance_;
  std::unique_ptr<MainWindow> main_window_;
  CefRefPtr<BrowserClient> browser_client_;
  std::wstring root_cache_path_;
  std::wstring cache_path_;
  bool startup_failed_ = false;

  IMPLEMENT_REFCOUNTING(App);
};

#endif  // GROKDESKTOP_SRC_APP_H_
