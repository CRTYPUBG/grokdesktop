# Grok Desktop

Windows masaüstünde [grok.com](https://grok.com/) adresini CEF (Chromium Embedded Framework) ile açan minimal bir C++20 uygulaması.

**Kaynak deposu:** [github.com/CRTYPUBG/grokdesktop](https://github.com/CRTYPUBG/grokdesktop.git)

## Durum

Uygulama geliştirildi ve derlendi. Release x64 çıktısı `build\src\Release\GrokDesktop.exe` konumundadır.

## Özellikler

- Yerel Win32 penceresi + CEF tarayıcı çocuk penceresi
- Koyu tema araç çubuğu (Geri, İleri, Yenile, Ana Sayfa — DPI-duyarlı, hover)
- Uygulama simgesi: başlık çubuğu, görev çubuğu ve pencere sınıfı
- Tek instance: ikinci kopya çalıştırılırsa mevcut pencere öne alınır
- Kalıcı profil: `%LOCALAPPDATA%\GrokDesktop\CEFProfile`
- Başlangıç URL’si: `https://grok.com/?utm_source=grokdesktop&utm_medium=desktop_app&utm_campaign=launch`
- Google / Apple / Microsoft / GitHub / X gibi OAuth girişleri **sistem tarayıcısında** (Chrome/Edge) açılır — gömülü webview’de engellenen girişler böylece çalışır
- `window.open` / diğer açılır pencereler ana pencerenin çocuğu olarak gömülür
- Yalnızca `https:`, `http:`, `about:`, `data:` URL’lerine izin verilir
- TLS sertifika hataları asla atlatılmaz
- Parola, çerez, token veya sayfa içeriği loglanmaz

## Derleme

Ön koşullar: Windows 10/11 x64, Visual Studio 2022 (C++ masaüstü iş yükü), CMake, resmi CEF binary dağıtımı (`third_party/cef/`).

```powershell
# Ortam kontrolü
.\scripts\check-environment.ps1

# Release x64 (sandbox açık) — bootstrap.exe’ye simge gömme dâhil
.\scripts\build.ps1

# Portable ZIP + Inno Setup yükleyicisi + SHA256SUMS
.\scripts\package.ps1
```

Ayrıntılı bilgi: [docs/BUILDING.md](docs/BUILDING.md)

## RISVM / koruma SDK’ları

Depoda `third_party/RISVM-sdk/` (anti-reverse SDK belgeleri) ve
`third_party/Include/` (Themida, VMProtect başlıkları) bulunur. Bunlar
**şu an derlemeye bağlı değildir**; lisans ve binary eksikliği nedeniyle
dağıtıma dâhil edilmez. İnceleme notları: [docs/RISVM-REVIEW.md](docs/RISVM-REVIEW.md)

## Güvenlik

- Grok parolanızı, oturum çerezlerinizi veya gerçek API anahtarlarını proje dosyalarına koymayın.
- Giriş, CAPTCHA veya site güvenlik önlemlerini atlatmayın.
- CEF ve Chromium lisans/dağıtım koşullarını kontrol edin.
- OAuth için varsayılan sistem tarayıcısı kullanılır; oturum tarayıcıda kalır.

## GitHub

Bu proje [CRTYPUBG/grokdesktop](https://github.com/CRTYPUBG/grokdesktop) deposunda barındırılır.

```bash
git clone https://github.com/CRTYPUBG/grokdesktop.git
cd grokdesktop
```
