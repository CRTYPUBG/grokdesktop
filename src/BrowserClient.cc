#include "src/BrowserClient.h"

#include <cstring>
#include <shellapi.h>
#include <sstream>
#include <string>

#include "include/base/cef_callback.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "src/app_config.h"
#include "src/MainWindow.h"

namespace {

bool HasPrefix(const std::string& value, const char* prefix) {
  const size_t prefix_length = std::strlen(prefix);
  return value.size() >= prefix_length &&
         value.compare(0, prefix_length, prefix) == 0;
}

bool Contains(const std::string& value, const char* needle) {
  return value.find(needle) != std::string::npos;
}

// Only allow web content and the internal blank/data pages that the app needs.
// Anything else (file:, javascript:, custom protocol handlers, etc.) is
// blocked so that untrusted page content cannot launch local code.
bool IsAllowedUrl(const CefString& url) {
  const std::string value = url.ToString();
  return HasPrefix(value, "https://") || HasPrefix(value, "http://") ||
         HasPrefix(value, "about:") || HasPrefix(value, "data:");
}

// Identity providers that block or degrade sign-in inside embedded webviews.
// Opening them in the OS default browser (Chrome/Edge) keeps OAuth working.
bool IsExternalAuthUrl(const CefString& url) {
  const std::string value = url.ToString();
  return Contains(value, "accounts.google.com") ||
         Contains(value, "appleid.apple.com") ||
         Contains(value, "login.microsoftonline.com") ||
         Contains(value, "login.live.com") ||
         Contains(value, "github.com/login") ||
         Contains(value, "x.com/i/oauth") ||
         Contains(value, "api.twitter.com/oauth") ||
         Contains(value, "facebook.com/dialog/oauth") ||
         Contains(value, "www.facebook.com/dialog/oauth") ||
         Contains(value, "discord.com/oauth2") ||
         Contains(value, "auth0.com") ||
         Contains(value, "okta.com");
}

void OpenInSystemBrowser(const std::string& url) {
  if (url.empty()) {
    return;
  }
  // ShellExecuteW uses the user's default browser (Chrome/Edge/Firefox).
  ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

std::string EscapeHtml(const std::string& input) {
  std::string output;
  output.reserve(input.size());
  for (const char c : input) {
    switch (c) {
      case '&':
        output += "&amp;";
        break;
      case '<':
        output += "&lt;";
        break;
      case '>':
        output += "&gt;";
        break;
      case '"':
        output += "&quot;";
        break;
      case '\'':
        output += "&#39;";
        break;
      default:
        output += c;
        break;
    }
  }
  return output;
}

std::string GetDataUri(const std::string& html) {
  return "data:text/html;base64," +
         CefBase64Encode(html.data(), html.size()).ToString();
}

std::string BuildErrorPage(const std::string& failed_url,
                           const std::string& error_text,
                           int error_code) {
  std::string text = error_text;
  if (text.empty()) {
    text = "Bilinmeyen ag hatasi";
  }
  std::string url = failed_url;
  if (url.empty()) {
    url = kStartupUrl;
  }

  std::ostringstream html;
  html << "<!doctype html><html lang=\"tr\"><head><meta charset=\"utf-8\">"
       << "<meta name=\"viewport\" content=\"width=device-width,"
          "initial-scale=1\">"
       << "<title>Grok acilamadi</title><style>"
       << "body{margin:0;background:#0d0d0d;color:#e8e8e8;"
          "font-family:'Segoe UI',Arial,sans-serif}"
       << ".card{max-width:560px;margin:10vh auto;padding:32px 36px;"
          "background:#1a1a1a;border:1px solid #2a2a2a;border-radius:12px}"
       << "h1{font-size:22px;margin:0 0 12px}"
       << "p{line-height:1.55;margin:0 0 12px}"
       << "code{color:#9aa0a6;word-break:break-all}"
       << "a.retry{display:inline-block;margin-top:8px;padding:9px 18px;"
          "background:#1a73e8;color:#fff;text-decoration:none;"
          "border-radius:6px}"
       << "</style></head><body><div class=\"card\">"
       << "<h1>Grok sayfasi acilamadi</h1>"
       << "<p>Uygulama <code>" << EscapeHtml(url)
       << "</code> adresine ulasamadi. Internet baglantinizi ve ag "
          "ayarlarinizi kontrol edip tekrar deneyin.</p>"
       << "<p><code>" << EscapeHtml(text) << " (" << error_code
       << ")</code></p>"
       << "<a class=\"retry\" href=\"" << EscapeHtml(kStartupUrl)
       << "\">Tekrar dene</a>"
       << "</div></body></html>";
  return html.str();
}

}  // namespace

BrowserClient::BrowserClient(MainWindow* main_window)
    : main_window_(main_window) {}

void BrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
  CEF_REQUIRE_UI_THREAD();
  if (main_browser_ && main_browser_->IsSame(browser)) {
    main_window_->OnTitleChange(title);
  }
}

void BrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool isLoading,
                                         bool canGoBack,
                                         bool canGoForward) {
  CEF_REQUIRE_UI_THREAD();
  if (main_browser_ && main_browser_->IsSame(browser)) {
    main_window_->OnLoadingStateChange(isLoading, canGoBack, canGoForward);
  }
}

void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  browser_list_.push_back(browser);
  if (!main_browser_) {
    main_browser_ = browser;
    main_window_->OnBrowserCreated(browser);
  }
}

bool BrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  if (main_browser_ && main_browser_->IsSame(browser)) {
    // The main window already cancelled its own close; allow the next one.
    is_closing_ = true;
    main_window_->AllowClose();
    // Returning false makes CEF send WM_CLOSE to the top-level parent window
    // (our main window), which then allows the destroy. See the
    // CefLifeSpanHandler close example 2.
    return false;
  }
  // Popup browsers are children of the main window; let them close immediately
  // without re-triggering the main window's close flow.
  return true;
}

void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  main_window_->OnBrowserClosed(browser);

  browser_list_.remove_if(
      [&browser](const CefRefPtr<CefBrowser>& item) { return item->IsSame(browser); });

  if (main_browser_ && main_browser_->IsSame(browser)) {
    main_browser_ = nullptr;
  }

  if (browser_list_.empty()) {
    // All browser windows are gone; exit the CEF message loop.
    main_window_->OnAllBrowsersClosed();
  }
}

void BrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Aborted loads (navigation cancelled, downloads, etc.) are not errors.
  if (errorCode == ERR_ABORTED) {
    return;
  }
  // Only the main frame gets the friendly error page.
  if (!frame->IsMain()) {
    return;
  }
  // Never try to load an error page into a data: URL.
  if (HasPrefix(failedUrl.ToString(), "data:")) {
    return;
  }

  frame->LoadURL(
      GetDataUri(BuildErrorPage(failedUrl.ToString(), errorText.ToString(),
                                static_cast<int>(errorCode))));
}

bool BrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   bool user_gesture,
                                   bool is_redirect) {
  CEF_REQUIRE_UI_THREAD();
  UNREFERENCED_PARAMETER(browser);
  UNREFERENCED_PARAMETER(frame);
  UNREFERENCED_PARAMETER(user_gesture);
  UNREFERENCED_PARAMETER(is_redirect);
  // Block non-web schemes. http(s) navigation to Grok's own auth domains is
  // intentionally left alone so that sign-in keeps working.
  return !IsAllowedUrl(request->GetURL());
}

bool BrowserClient::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     const CefString& target_url,
                                     cef_window_open_disposition_t
                                         target_disposition,
                                     bool user_gesture) {
  CEF_REQUIRE_UI_THREAD();
  UNREFERENCED_PARAMETER(browser);
  UNREFERENCED_PARAMETER(frame);
  UNREFERENCED_PARAMETER(target_disposition);
  UNREFERENCED_PARAMETER(user_gesture);
  // Returning false keeps the navigation in the existing (single) window.
  return !IsAllowedUrl(target_url);
}

bool BrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  int popup_id,
                                  const CefString& target_url,
                                  const CefString& target_frame_name,
                                  cef_window_open_disposition_t
                                      target_disposition,
                                  bool user_gesture,
                                  const CefPopupFeatures& popupFeatures,
                                  CefWindowInfo& windowInfo,
                                  CefRefPtr<CefClient>& client,
                                  CefBrowserSettings& settings,
                                  CefRefPtr<CefDictionaryValue>& extra_info,
                                  bool* no_javascript_access) {
  CEF_REQUIRE_UI_THREAD();
  UNREFERENCED_PARAMETER(browser);
  UNREFERENCED_PARAMETER(frame);
  UNREFERENCED_PARAMETER(popup_id);
  UNREFERENCED_PARAMETER(target_frame_name);
  UNREFERENCED_PARAMETER(target_disposition);
  UNREFERENCED_PARAMETER(user_gesture);
  UNREFERENCED_PARAMETER(popupFeatures);
  UNREFERENCED_PARAMETER(windowInfo);
  UNREFERENCED_PARAMETER(extra_info);
  UNREFERENCED_PARAMETER(no_javascript_access);

  if (!IsAllowedUrl(target_url)) {
    return true;  // Cancel.
  }

  // Google / Apple / Microsoft / etc. block or break OAuth inside embedded
  // webviews. Open those sign-in popups in the OS default browser instead.
  if (IsExternalAuthUrl(target_url)) {
    OpenInSystemBrowser(target_url.ToString());
    return true;  // Cancel the in-app popup.
  }

  // Embed remaining popups as a child of the main window so the app stays a
  // single native window (window.open flows still get their own browser).
  // Size it over the browser content area below the toolbar.
  if (main_window_ && main_window_->hwnd()) {
    RECT rc = {};
    GetClientRect(main_window_->hwnd(), &rc);
    const int toolbar_height = main_window_->ToolbarHeight();
    const int width = static_cast<int>(rc.right);
    const int client_height = static_cast<int>(rc.bottom);
    const int height = (client_height > toolbar_height)
                           ? (client_height - toolbar_height)
                           : 0;
    windowInfo.SetAsChild(main_window_->hwnd(),
                          CefRect(0, toolbar_height, width, height));
    windowInfo.runtime_style = CEF_RUNTIME_STYLE_DEFAULT;
  }
  // Keep routing callbacks through this client so the popup is tracked and
  // closed with the app.
  client = this;
  return false;
}

bool BrowserClient::OnCertificateError(CefRefPtr<CefBrowser> browser,
                                       cef_errorcode_t cert_error,
                                       const CefString& request_url,
                                       CefRefPtr<CefSSLInfo> ssl_info,
                                       CefRefPtr<CefCallback> callback) {
  CEF_REQUIRE_UI_THREAD();
  UNREFERENCED_PARAMETER(browser);
  UNREFERENCED_PARAMETER(cert_error);
  UNREFERENCED_PARAMETER(request_url);
  UNREFERENCED_PARAMETER(ssl_info);
  UNREFERENCED_PARAMETER(callback);
  // Never bypass TLS validation; cancel the request immediately.
  return false;
}

void BrowserClient::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    CefPostTask(TID_UI, base::BindOnce(&BrowserClient::CloseAllBrowsers, this,
                                       force_close));
    return;
  }
  if (browser_list_.empty()) {
    return;
  }
  for (const auto& browser : browser_list_) {
    browser->GetHost()->CloseBrowser(force_close);
  }
}
