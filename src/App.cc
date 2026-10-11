#include "src/App.h"

#include <shlobj.h>

#include <algorithm>

#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "src/app_config.h"
#include "src/BrowserClient.h"
#include "src/MainWindow.h"

namespace {

std::wstring GetLocalAppDataDirectory() {
  wchar_t buffer[MAX_PATH] = {};
  if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr,
                                 SHGFP_TYPE_CURRENT, buffer))) {
    return buffer;
  }
  // Fallback so the app still starts (and reports) without a profile dir.
  wchar_t temp[MAX_PATH] = {};
  const DWORD length = GetTempPathW(MAX_PATH, temp);
  if (length > 0 && length < MAX_PATH) {
    return std::wstring(temp, length);
  }
  return L".";
}

std::wstring JoinPath(const std::wstring& directory, const wchar_t* name) {
  if (directory.empty()) {
    return name;
  }
  if (directory.back() == L'\\' || directory.back() == L'/') {
    return directory + name;
  }
  return directory + L"\\" + name;
}

bool EnsureDirectoryTree(const std::wstring& path) {
  if (path.empty()) {
    return false;
  }
  const DWORD attributes = GetFileAttributesW(path.c_str());
  if (attributes != INVALID_FILE_ATTRIBUTES) {
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
  }
  const size_t separator = path.find_last_of(L"\\/");
  if (separator != std::wstring::npos && separator > 0) {
    if (!EnsureDirectoryTree(path.substr(0, separator))) {
      return false;
    }
  }
  if (CreateDirectoryW(path.c_str(), nullptr)) {
    return true;
  }
  return GetLastError() == ERROR_ALREADY_EXISTS;
}

}  // namespace

App::App(HINSTANCE hinstance) : hinstance_(hinstance) {}

bool App::InitializeProfilePaths() {
  std::wstring root = GetLocalAppDataDirectory();
  while (!root.empty() && (root.back() == L'\\' || root.back() == L'/')) {
    root.pop_back();
  }
  root_cache_path_ = JoinPath(root, L"GrokDesktop");
  cache_path_ = JoinPath(root_cache_path_, L"CEFProfile");

  if (!EnsureDirectoryTree(root_cache_path_)) {
    return false;
  }
  if (!EnsureDirectoryTree(cache_path_)) {
    return false;
  }
  return true;
}

void App::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  main_window_ = std::make_unique<MainWindow>(hinstance_);
  if (!main_window_->Create()) {
    MessageBoxW(nullptr,
                L"Grok Desktop ana penceresi olusturulamadi.\n"
                L"Lutfen Windows surumunuzu kontrol edin.",
                kAppName, MB_OK | MB_ICONERROR);
    startup_failed_ = true;
    return;
  }

  browser_client_ = new BrowserClient(main_window_.get());
  if (!main_window_->CreateBrowser(kStartupUrl, browser_client_)) {
    MessageBoxW(nullptr,
                L"CEF browser penceresi olusturulamadi.\n"
                L"Kurulum eksik olabilir; docs/BUILDING.md dosyasina bakin.",
                kAppName, MB_OK | MB_ICONERROR);
    startup_failed_ = true;
    return;
  }
}

bool App::OnAlreadyRunningAppRelaunch(
    CefRefPtr<CefCommandLine> command_line,
    const CefString& current_directory) {
  CEF_REQUIRE_UI_THREAD();
  UNREFERENCED_PARAMETER(command_line);
  UNREFERENCED_PARAMETER(current_directory);

  // The profile is locked by an existing instance: just bring that window to
  // the front and let this launch exit early.
  if (main_window_) {
    main_window_->Activate();
  }
  return true;
}

CefRefPtr<CefClient> App::GetDefaultClient() {
  // Used for browser windows created via Chrome style UI.
  return browser_client_;
}
