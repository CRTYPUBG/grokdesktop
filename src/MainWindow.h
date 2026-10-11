// Win32 top-level window hosting the CEF browser child window plus a small
// navigation toolbar (Back / Forward / Reload / Home).
#ifndef GROKDESKTOP_SRC_MAIN_WINDOW_H_
#define GROKDESKTOP_SRC_MAIN_WINDOW_H_

#include <windows.h>

#include <string>

#include "include/cef_browser.h"
#include "src/BrowserClient.h"

// Owns the native top-level window. All methods run on the browser process UI
// thread, which is also the thread that owns the window.
class MainWindow {
 public:
  // Registers the window class once per process.
  static bool RegisterWindowClass(HINSTANCE hinstance);

  explicit MainWindow(HINSTANCE hinstance);
  ~MainWindow();

  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

  // Creates and shows the top-level window.
  bool Create();

  // Creates the CEF browser as a child of the client area below the toolbar.
  bool CreateBrowser(const std::string& url, CefRefPtr<BrowserClient> client);

  HWND hwnd() const { return hwnd_; }
  bool HasBrowser() const { return browser_ != nullptr; }

  // Current toolbar height in pixels (DPI-scaled). Used by BrowserClient when
  // embedding popups below the toolbar.
  int ToolbarHeight() const;

  // BrowserClient callbacks (CEF UI thread == window thread).
  void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
  void OnTitleChange(const CefString& title);
  void OnLoadingStateChange(bool is_loading,
                            bool can_go_back,
                            bool can_go_forward);
  void OnBrowserClosed(CefRefPtr<CefBrowser> browser);

  // Called when the last browser has been closed; quits the message loop.
  void OnAllBrowsersClosed();

  // Called by DoClose for the main browser so the next WM_CLOSE is allowed.
  void AllowClose();

  // Brings the window to the foreground (used for the single-instance path).
  void Activate();

 private:
  static LRESULT CALLBACK WndProc(HWND hwnd,
                                  UINT message,
                                  WPARAM wparam,
                                  LPARAM lparam);
  LRESULT HandleMessage(UINT message, WPARAM wparam, LPARAM lparam);

  void CreateToolbarControls();
  void Layout();
  void UpdateNavigationState();
  void SetBrowserTitle(const std::wstring& title);
  HINSTANCE ResourceInstance() const;

  int Scale(int value) const;

  HINSTANCE const hinstance_;
  CefRefPtr<BrowserClient> browser_client_;  // set by CreateBrowser
  HWND hwnd_ = nullptr;
  HWND back_button_ = nullptr;
  HWND forward_button_ = nullptr;
  HWND reload_button_ = nullptr;
  HWND home_button_ = nullptr;
  HFONT toolbar_font_ = nullptr;
  HBRUSH toolbar_brush_ = nullptr;
  CefRefPtr<CefBrowser> browser_;
  bool is_loading_ = false;
  bool can_go_back_ = false;
  bool can_go_forward_ = false;
  bool close_allowed_ = false;
  bool is_closing_ = false;
};

#endif  // GROKDESKTOP_SRC_MAIN_WINDOW_H_
