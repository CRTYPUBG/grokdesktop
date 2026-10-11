#include "src/MainWindow.h"

#include <dwmapi.h>
#include <windowsx.h>

#include <algorithm>

#include "include/cef_app.h"
#include "resources/resource.h"
#include "src/app_config.h"
#include "src/BrowserClient.h"

namespace {

const wchar_t kWindowClassName[] = L"GrokDesktopMainWindow";

// Toolbar metrics in 96-DPI units; scaled per window DPI.
constexpr int kToolbarHeight = 44;
constexpr int kButtonWidth = 92;
constexpr int kButtonHeight = 30;
constexpr int kButtonGap = 6;
constexpr int kButtonMarginY = 7;
constexpr int kButtonMarginX = 10;

constexpr int kDefaultWidth = 1180;
constexpr int kDefaultHeight = 820;

// Grok-like dark chrome so the toolbar blends with the page background.
constexpr COLORREF kToolbarBg = RGB(13, 13, 13);
constexpr COLORREF kButtonFace = RGB(32, 32, 32);
constexpr COLORREF kButtonFaceHover = RGB(48, 48, 48);
constexpr COLORREF kButtonFaceDown = RGB(64, 64, 64);
constexpr COLORREF kButtonFaceDisabled = RGB(22, 22, 22);
constexpr COLORREF kButtonText = RGB(232, 232, 232);
constexpr COLORREF kButtonTextDisabled = RGB(110, 110, 110);

bool g_class_registered = false;
HBRUSH g_toolbar_brush = nullptr;
HWND g_hot_button = nullptr;

void UpdateHotButton(HWND parent, POINT screen_pt) {
  HWND buttons[] = {GetDlgItem(parent, IDC_NAV_BACK),
                    GetDlgItem(parent, IDC_NAV_FORWARD),
                    GetDlgItem(parent, IDC_NAV_RELOAD),
                    GetDlgItem(parent, IDC_NAV_HOME)};
  HWND hot = nullptr;
  for (HWND button : buttons) {
    if (!button) {
      continue;
    }
    RECT rc = {};
    GetWindowRect(button, &rc);
    if (PtInRect(&rc, screen_pt)) {
      hot = button;
      break;
    }
  }
  if (hot == g_hot_button) {
    return;
  }
  if (g_hot_button) {
    InvalidateRect(g_hot_button, nullptr, TRUE);
  }
  g_hot_button = hot;
  if (g_hot_button) {
    InvalidateRect(g_hot_button, nullptr, TRUE);
  }
}

}  // namespace

// static
bool MainWindow::RegisterWindowClass(HINSTANCE hinstance) {
  if (g_class_registered) {
    return true;
  }

  if (!g_toolbar_brush) {
    g_toolbar_brush = CreateSolidBrush(kToolbarBg);
  }

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = MainWindow::WndProc;
  wc.hInstance = hinstance;
  // Icons live in GrokDesktop.dll; bootstrap.exe has none of its own, so
  // prefer the DLL module when this code runs under the sandbox launcher.
  HINSTANCE icon_instance = GetModuleHandleW(L"GrokDesktop.dll");
  if (!icon_instance) {
    icon_instance = hinstance;
  }
  wc.hIcon = LoadIconW(icon_instance, MAKEINTRESOURCEW(IDI_GROKDESKTOP));
  wc.hIconSm = LoadIconW(icon_instance, MAKEINTRESOURCEW(IDI_SMALL));
  wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  // Dark class brush matches the toolbar and avoids a white flash on create.
  wc.hbrBackground = g_toolbar_brush;
  wc.lpszClassName = kWindowClassName;

  if (RegisterClassExW(&wc) == 0) {
    return false;
  }
  g_class_registered = true;
  return true;
}

MainWindow::MainWindow(HINSTANCE hinstance) : hinstance_(hinstance) {}

MainWindow::~MainWindow() {
  if (hwnd_) {
    DestroyWindow(hwnd_);
    hwnd_ = nullptr;
  }
  if (toolbar_font_) {
    DeleteObject(toolbar_font_);
    toolbar_font_ = nullptr;
  }
  // brush is process-global (g_toolbar_brush); do not delete per-window.
}

bool MainWindow::Create() {
  if (!RegisterWindowClass(hinstance_)) {
    return false;
  }

  RECT work_area = {};
  SystemParametersInfoW(SPI_GETWORKAREA, 0, &work_area, 0);
  const int work_width = work_area.right - work_area.left;
  const int work_height = work_area.bottom - work_area.top;
  const int width = std::min(kDefaultWidth, work_width);
  const int height = std::min(kDefaultHeight, work_height);
  const int x = work_area.left + (work_width - width) / 2;
  const int y = work_area.top + (work_height - height) / 2;

  hwnd_ = CreateWindowExW(0, kWindowClassName, kAppName,
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, x, y, width,
                          height, nullptr, nullptr, hinstance_, this);
  if (!hwnd_) {
    return false;
  }

  // Explicitly set icons so the title bar / taskbar show the app logo even
  // when the process executable is CEF's stock bootstrap.exe.
  HINSTANCE icon_instance = ResourceInstance();
  HICON big_icon = LoadIconW(icon_instance, MAKEINTRESOURCEW(IDI_GROKDESKTOP));
  HICON small_icon = LoadIconW(icon_instance, MAKEINTRESOURCEW(IDI_SMALL));
  if (big_icon) {
    SendMessageW(hwnd_, WM_SETICON, ICON_BIG,
                 reinterpret_cast<LPARAM>(big_icon));
  }
  if (small_icon) {
    SendMessageW(hwnd_, WM_SETICON, ICON_SMALL,
                 reinterpret_cast<LPARAM>(small_icon));
  }

  // Dark title bar (Win10 20H1+ / Win11). Best-effort; ignore failures on
  // older builds that do not support the attribute.
  const BOOL dark = TRUE;
  constexpr DWORD kDwmwaUseImmersiveDarkMode = 20;
  constexpr DWORD kDwmwaUseImmersiveDarkModeBefore20H1 = 19;
  DwmSetWindowAttribute(hwnd_, kDwmwaUseImmersiveDarkMode, &dark,
                        sizeof(dark));
  DwmSetWindowAttribute(hwnd_, kDwmwaUseImmersiveDarkModeBefore20H1, &dark,
                        sizeof(dark));

  CreateToolbarControls();
  Layout();

  ShowWindow(hwnd_, SW_SHOW);
  UpdateWindow(hwnd_);
  return true;
}

bool MainWindow::CreateBrowser(const std::string& url,
                               CefRefPtr<BrowserClient> client) {
  if (!hwnd_ || !client) {
    return false;
  }
  browser_client_ = client;

  RECT rc = {};
  GetClientRect(hwnd_, &rc);
  const int client_width = static_cast<int>(rc.right);
  const int client_height = static_cast<int>(rc.bottom);
  const int toolbar_height = Scale(kToolbarHeight);

  CefWindowInfo window_info;
  window_info.SetAsChild(
      hwnd_, CefRect(0, toolbar_height, client_width,
                     (client_height > toolbar_height)
                         ? (client_height - toolbar_height)
                         : 0));
  // Chrome runtime style is the default for this CEF version.
  window_info.runtime_style = CEF_RUNTIME_STYLE_DEFAULT;

  CefBrowserSettings browser_settings;
  return CefBrowserHost::CreateBrowser(window_info, browser_client_, url,
                                       browser_settings, nullptr, nullptr);
}

void MainWindow::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  browser_ = browser;
  is_loading_ = true;
  Layout();
  UpdateNavigationState();
  if (browser_) {
    browser_->GetHost()->SetFocus(true);
  }
}

void MainWindow::OnTitleChange(const CefString& title) {
  SetBrowserTitle(title.ToWString());
}

void MainWindow::OnLoadingStateChange(bool is_loading,
                                      bool can_go_back,
                                      bool can_go_forward) {
  is_loading_ = is_loading;
  can_go_back_ = can_go_back;
  can_go_forward_ = can_go_forward;
  UpdateNavigationState();
}

void MainWindow::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
  if (browser_ && browser_->IsSame(browser)) {
    browser_ = nullptr;
  }
}

void MainWindow::OnAllBrowsersClosed() {
  browser_ = nullptr;
  CefQuitMessageLoop();
}

void MainWindow::AllowClose() {
  close_allowed_ = true;
}

void MainWindow::Activate() {
  if (!hwnd_) {
    return;
  }
  if (IsIconic(hwnd_)) {
    ShowWindow(hwnd_, SW_RESTORE);
  }
  SetForegroundWindow(hwnd_);
}

// static
LRESULT CALLBACK MainWindow::WndProc(HWND hwnd,
                                     UINT message,
                                     WPARAM wparam,
                                     LPARAM lparam) {
  MainWindow* self = nullptr;
  if (message == WM_NCCREATE) {
    auto* create_struct = reinterpret_cast<CREATESTRUCTW*>(lparam);
    self = static_cast<MainWindow*>(create_struct->lpCreateParams);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    self->hwnd_ = hwnd;
  } else if (message != WM_NCDESTROY) {
    self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
  }

  if (self) {
    return self->HandleMessage(message, wparam, lparam);
  }
  return DefWindowProcW(hwnd, message, wparam, lparam);
}

LRESULT MainWindow::HandleMessage(UINT message,
                                  WPARAM wparam,
                                  LPARAM lparam) {
  switch (message) {
    case WM_COMMAND: {
      const int id = LOWORD(wparam);
      if (!browser_) {
        return 0;
      }
      switch (id) {
        case IDC_NAV_BACK:
          browser_->GoBack();
          return 0;
        case IDC_NAV_FORWARD:
          browser_->GoForward();
          return 0;
        case IDC_NAV_RELOAD:
          browser_->Reload();
          return 0;
        case IDC_NAV_HOME:
          browser_->GetMainFrame()->LoadURL(kStartupUrl);
          return 0;
        default:
          break;
      }
      return 0;
    }

    case WM_MOUSEMOVE: {
      TRACKMOUSEEVENT tme = {};
      tme.cbSize = sizeof(tme);
      tme.dwFlags = TME_LEAVE;
      tme.hwndTrack = hwnd_;
      TrackMouseEvent(&tme);
      POINT pt = {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)};
      ClientToScreen(hwnd_, &pt);
      UpdateHotButton(hwnd_, pt);
      return 0;
    }

    case WM_MOUSELEAVE: {
      if (g_hot_button) {
        InvalidateRect(g_hot_button, nullptr, TRUE);
        g_hot_button = nullptr;
      }
      return 0;
    }

    case WM_DRAWITEM: {
      auto* draw = reinterpret_cast<DRAWITEMSTRUCT*>(lparam);
      if (!draw || draw->CtlType != ODT_BUTTON) {
        break;
      }
      const UINT state = draw->itemState;
      RECT rc = draw->rcItem;

      COLORREF face = kButtonFace;
      if (state & ODS_DISABLED) {
        face = kButtonFaceDisabled;
      } else if (state & ODS_SELECTED) {
        face = kButtonFaceDown;
      } else if (draw->hwndItem == g_hot_button) {
        face = kButtonFaceHover;
      }

      HBRUSH brush = CreateSolidBrush(face);
      FillRect(draw->hDC, &rc, brush);
      DeleteObject(brush);

      // 1px inner border for definition against the dark toolbar.
      HPEN pen = CreatePen(PS_SOLID, 1, RGB(58, 58, 58));
      HGDIOBJ old_pen = SelectObject(draw->hDC, pen);
      HGDIOBJ old_brush = SelectObject(draw->hDC, GetStockObject(NULL_BRUSH));
      Rectangle(draw->hDC, rc.left, rc.top, rc.right, rc.bottom);
      SelectObject(draw->hDC, old_pen);
      SelectObject(draw->hDC, old_brush);
      DeleteObject(pen);

      wchar_t text[64] = {};
      GetWindowTextW(draw->hwndItem, text, 64);
      SetBkMode(draw->hDC, TRANSPARENT);
      SetTextColor(draw->hDC,
                   (state & ODS_DISABLED) ? kButtonTextDisabled : kButtonText);
      HFONT font = toolbar_font_
                       ? toolbar_font_
                       : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
      HGDIOBJ old_font = SelectObject(draw->hDC, font);
      RECT text_rc = rc;
      DrawTextW(draw->hDC, text, -1, &text_rc,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      SelectObject(draw->hDC, old_font);

      if (state & ODS_FOCUS) {
        RECT focus_rc = rc;
        InflateRect(&focus_rc, -3, -3);
        DrawFocusRect(draw->hDC, &focus_rc);
      }
      return TRUE;
    }

    case WM_CTLCOLORSTATIC: {
      HDC hdc = reinterpret_cast<HDC>(wparam);
      SetTextColor(hdc, kButtonText);
      SetBkColor(hdc, kToolbarBg);
      return reinterpret_cast<LRESULT>(g_toolbar_brush);
    }

    case WM_ERASEBKGND: {
      HDC hdc = reinterpret_cast<HDC>(wparam);
      RECT rc = {};
      GetClientRect(hwnd_, &rc);
      FillRect(hdc, &rc, g_toolbar_brush);
      return 1;
    }

    case WM_SIZE: {
      if (wparam == SIZE_MINIMIZED && browser_) {
        // Shrink the browser to 0x0 while minimized to reduce resource usage.
        HWND browser_hwnd = browser_->GetHost()->GetWindowHandle();
        if (browser_hwnd) {
          SetWindowPos(browser_hwnd, nullptr, 0, 0, 0, 0,
                       SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        }
        return 0;
      }
      Layout();
      return 0;
    }

    case WM_DPICHANGED: {
      Layout();
      return 0;
    }

    case WM_SETFOCUS:
      if (browser_) {
        browser_->GetHost()->SetFocus(true);
      }
      return 0;

    case WM_CLOSE:
      if (!close_allowed_) {
        if (browser_ && !is_closing_) {
          is_closing_ = true;
          // Give JavaScript unload handlers a chance to run; DoClose() then
          // re-sends WM_CLOSE which we allow. See the CefLifeSpanHandler
          // documentation (close example 2).
          browser_client_->CloseAllBrowsers(false);
        }
        return 0;
      }
      return DefWindowProcW(hwnd_, message, wparam, lparam);

    case WM_DESTROY:
      return 0;

    case WM_NCDESTROY: {
      LRESULT result = DefWindowProcW(hwnd_, message, wparam, lparam);
      SetWindowLongPtrW(hwnd_, GWLP_USERDATA, 0);
      hwnd_ = nullptr;
      return result;
    }

    default:
      break;
  }
  return DefWindowProcW(hwnd_, message, wparam, lparam);
}

HINSTANCE MainWindow::ResourceInstance() const {
  HINSTANCE icon_instance = GetModuleHandleW(L"GrokDesktop.dll");
  return icon_instance ? icon_instance : hinstance_;
}

void MainWindow::CreateToolbarControls() {
  if (!toolbar_font_) {
    toolbar_font_ = CreateFontW(-Scale(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                                FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  }

  auto create_button = [this](int id, const wchar_t* text) -> HWND {
    HWND button = CreateWindowExW(
        0, L"BUTTON", text,
        WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_OWNERDRAW | BS_CENTER |
            BS_VCENTER,
        0, 0, Scale(kButtonWidth), Scale(kButtonHeight), hwnd_,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hinstance_,
        nullptr);
    if (button && toolbar_font_) {
      SendMessageW(button, WM_SETFONT, reinterpret_cast<WPARAM>(toolbar_font_),
                   TRUE);
    }
    return button;
  };

  back_button_ = create_button(IDC_NAV_BACK, L"\x2190  Geri");
  forward_button_ = create_button(IDC_NAV_FORWARD, L"\x2192  \u0130leri");
  reload_button_ = create_button(IDC_NAV_RELOAD, L"Yenile");
  home_button_ = create_button(IDC_NAV_HOME, L"Ana Sayfa");
}

void MainWindow::Layout() {
  if (!hwnd_) {
    return;
  }

  RECT rc = {};
  GetClientRect(hwnd_, &rc);
  const int toolbar_height = Scale(kToolbarHeight);
  const int button_y = Scale(kButtonMarginY);
  const int button_width = Scale(kButtonWidth);
  const int button_height = Scale(kButtonHeight);
  const int gap = Scale(kButtonGap);

  int x = Scale(kButtonMarginX);
  auto place = [&](HWND button) {
    if (button) {
      SetWindowPos(button, nullptr, x, button_y, button_width, button_height,
                   SWP_NOZORDER | SWP_NOACTIVATE);
    }
    x += button_width + gap;
  };

  place(back_button_);
  place(forward_button_);
  place(reload_button_);
  place(home_button_);

  if (browser_) {
    HWND browser_hwnd = browser_->GetHost()->GetWindowHandle();
    if (browser_hwnd) {
      const int client_height = static_cast<int>(rc.bottom);
      const int client_width = static_cast<int>(rc.right);
      const int browser_height = (client_height > toolbar_height)
                                     ? (client_height - toolbar_height)
                                     : 0;
      SetWindowPos(browser_hwnd, nullptr, 0, toolbar_height, client_width,
                   browser_height, SWP_NOZORDER | SWP_NOACTIVATE);
    }
  }
}

void MainWindow::UpdateNavigationState() {
  if (back_button_) {
    EnableWindow(back_button_, browser_ != nullptr && can_go_back_);
  }
  if (forward_button_) {
    EnableWindow(forward_button_, browser_ != nullptr && can_go_forward_);
  }
  if (reload_button_) {
    EnableWindow(reload_button_, browser_ != nullptr);
  }
  if (home_button_) {
    EnableWindow(home_button_, browser_ != nullptr && !is_loading_);
  }
}

void MainWindow::SetBrowserTitle(const std::wstring& title) {
  if (!hwnd_) {
    return;
  }
  if (title.empty()) {
    SetWindowTextW(hwnd_, kAppName);
    return;
  }
  std::wstring full = title;
  full += L" - ";
  full += kAppName;
  SetWindowTextW(hwnd_, full.c_str());
}

int MainWindow::Scale(int value) const {
  UINT dpi = 96;
  if (hwnd_) {
    const UINT window_dpi = GetDpiForWindow(hwnd_);
    if (window_dpi != 0) {
      dpi = window_dpi;
    }
  }
  return MulDiv(value, static_cast<int>(dpi), 96);
}

int MainWindow::ToolbarHeight() const {
  return Scale(kToolbarHeight);
}
