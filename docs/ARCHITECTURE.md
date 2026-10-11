# Mimari

## Akış
WinMain / wWinMain
→ CEF initialize
→ Win32 main window
→ CEF browser child window
→ `https://grok.com/`
→ close browser
→ CEF shutdown

## Parçalar
- `App`: CEF application callbacks ve yaşam döngüsü.
- `BrowserClient`: load/display/lifespan callbacks; hassas web içeriğini loglamaz.
- `MainWindow`: Win32 pencere, toolbar, layout ve resize.
- `main.cpp`: DPI/instance setup, CEF subprocess handling ve uygulama girişi.
- `scripts`: configure/build/package işlemleri.
- `installer`: Inno Setup tanımı.

İsimler öneridir; seçilen CEF sürümünün API örüntüleri önceliklidir. CEF'in kendi CMake helper'ları varsa onlarla uyumlu kullan.
