// Application-wide constants shared by the browser process code.
#ifndef GROKDESKTOP_SRC_APP_CONFIG_H_
#define GROKDESKTOP_SRC_APP_CONFIG_H_

// The only startup target. UTM tags identify this desktop app to Grok.
// Grok may redirect to its own authentication domains during normal sign-in;
// those navigations are left untouched.
inline constexpr char kStartupUrl[] =
    "https://grok.com/?utm_source=grokdesktop&utm_medium=desktop_app"
    "&utm_campaign=launch";

inline constexpr wchar_t kAppName[] = L"Grok Desktop";
inline constexpr char kAppNameA[] = "Grok Desktop";

// Per-user data root: %LOCALAPPDATA%\GrokDesktop\CEFProfile
inline constexpr char kProfileRootDirName[] = "GrokDesktop";
inline constexpr char kProfileCacheDirName[] = "CEFProfile";

#endif  // GROKDESKTOP_SRC_APP_CONFIG_H_
