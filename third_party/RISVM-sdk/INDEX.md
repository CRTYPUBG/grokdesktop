# RISVM SDK - Dosya Dizini ve Hızlı Başvuru

## 📂 Dosya Yapısı

```
sdk/
│
├── 📄 README.md                        ← Başlangıç: SDK Özeti
├── 📄 INDEX.md                         ← Bu dosya
│
├── dist/                               ← Dağıtım Paketleri (DLL, LIB vb.)
│   ├── ADD.md                          ⭐ TÜRKÇE KONU: Nasıl Entegre Edilir?
│   ├── INSTALL.ps1                     ← PowerShell Kurulum Betiği
│   ├── dll/                            ← Runtime DLL dosyaları
│   │   ├── risvm_core.dll              ← Ana koruma sistemi
│   │   ├── risvm_electron.dll          ← Electron entegrasyonu
│   │   ├── risvm_crypto.dll            ← Şifreleme motoru
│   │   ├── watchdog.exe                ← İzleme ve yeniden başlatma
│   │   └── manifest-generator.exe
│   │
│   ├── lib/                            ← C++ Linking Kitaplıkları
│   │   ├── risvm_core.lib
│   │   ├── risvm_electron.lib
│   │   └── risvm_crypto.lib
│   │
│   ├── include/                        ← C/C++ Header Dosyaları
│   │   ├── compiler/
│   │   │   └── xrtvm_compiler.h        ← XRTVM Derleyici API
│   │   ├── runtime/
│   │   │   └── xrtvm_runtime.h         ← XRTVM Çalışma Zamanı API
│   │   └── risvm_core.h                ← RISVM Ana API
│   │
│   ├── config/                         ← Yapılandırma Şablonları
│   │   ├── risvm.default.json          ← Varsayılan config
│   │   ├── risvm.high-security.json    ← Yüksek güvenlik
│   │   └── risvm.low-impact.json       ← Düşük kaynak kullanımı
│   │
│   └── tools/                          ← Komut Satırı Araçları
│       ├── xrtvm-cli.exe               ← CLI aracı
│       └── xrtvm-cli-help.txt
│
├── examples/                           ← Kod Örnekleri
│   ├── CSharp_BasicIntegration/        ⭐ TÜRKÇE: Basit C# Örneği
│   │   ├── Program.cs                  ← Ana program
│   │   ├── RISVMWrapper.cs             ← DLL sarıcı sınıf
│   │   ├── risvm.default.json          ← Config dosyası
│   │   └── README.md                   ← Açıklama
│   │
│   ├── CPlusPlus_Advanced/             ⭐ İleri Seviye C++ Örneği
│   │   ├── main.cpp
│   │   ├── CMakeLists.txt
│   │   └── README.md
│   │
│   ├── Electron_Demo/                  ⭐ Electron.js Entegrasyonu
│   │   ├── main.js
│   │   ├── preload.js
│   │   ├── package.json
│   │   └── README.md
│   │
│   └── WindowsService/                 ⭐ Windows Hizmet Örneği
│       ├── RISVMService.cs
│       ├── App.config
│       └── README.md
│
├── compiler/                           ← XRTVM Derleyici SDK
│   ├── include/
│   │   └── xrtvm_compiler.h
│   └── lib/
│       └── xrtvm_compiler.lib
│
├── runtime/                            ← XRTVM Çalışma Zamanı SDK
│   ├── include/
│   │   └── xrtvm_runtime.h
│   └── lib/
│       └── xrtvm_runtime.lib
│
├── cmake/                              ← CMake Entegrasyonu
│   └── XRTVMConfig.cmake               ← CMake find_package desteği
│
├── vs-extension/                       ← Visual Studio Uzantısı
│   ├── XRTVMPackage.cs                 ← VS package sınıfı
│   ├── package.json
│   └── source.extension.vsixmanifest   ← VSIX manifesti
│
└── docs/                               ← Ek Belgeler (gelecek)
    ├── API_REFERENCE.md
    ├── PERFORMANCE.md
    ├── TROUBLESHOOTING.md
    └── FAQ.md
```

---

## 🎯 Hızlı Başvuru

### "Başlangıç yapmak istiyorum"
👉 **Adım 1:** `README.md` oku  
👉 **Adım 2:** `dist/ADD.md` oku (TÜRKÇE!)  
👉 **Adım 3:** `examples/CSharp_BasicIntegration/` çalıştır

### "C# projesine entegre etmek istiyorum"
👉 `dist/ADD.md` → Seçenek 1 (C# Örneği)  
👉 `examples/CSharp_BasicIntegration/Program.cs` kopyala  
👉 `dist/config/risvm.default.json` projeye kopyala

### "C++ projesine entegre etmek istiyorum"
👉 `dist/ADD.md` → Seçenek 2 (C++ Örneği)  
👉 `cmake/XRTVMConfig.cmake` kütüphanenize ekle  
👉 `examples/CPlusPlus_Advanced/` inceleniz

### "Electron uygulamasında kullanmak istiyorum"
👉 `dist/ADD.md` → Seçenek 4 (Electron)  
👉 `examples/Electron_Demo/main.js` kopyala  
👉 N-API bindings yükle

### "Uygulamayı Windows Hizmeti olarak çalıştırmak istiyorum"
👉 `examples/WindowsService/RISVMService.cs` kullan  
👉 `sc create RISVMService binPath= "C:\path\to\service.exe"` ile kaydı yap

### "DLL'leri dağıtmak istiyorum"
👉 `dist/dll/` klasörünün içeriğini programın `bin/` klasörüne kopyala  
👉 ya da `dist/INSTALL.ps1` çalıştır

### "Yapılandırmayı özelleştirmek istiyorum"
👉 `dist/config/risvm.default.json` düzenle  
👉 `dist/config/risvm.high-security.json` (yüksek güvenlik için)  
👉 `dist/config/risvm.low-impact.json` (düşük kaynak kullanımı için)

---

## 📚 Belge Haritası

| Amaç | Dosya | Dil |
|------|-------|-----|
| **SDK'yı anlamak** | README.md | İngilizce |
| **Entegrasyon yapılı adımlar** | dist/ADD.md | 🇹🇷 TÜRKÇE |
| **C# kodu örneği** | examples/CSharp_BasicIntegration/ | İngilizce + Açıklamalar |
| **C++ kodu örneği** | examples/CPlusPlus_Advanced/ | İngilizce |
| **Electron örneği** | examples/Electron_Demo/ | İngilizce |
| **Windows Service örneği** | examples/WindowsService/ | İngilizce |
| **API Referansı** | compiler/include/xrtvm_compiler.h | C++ |
| **Runtime API** | runtime/include/xrtvm_runtime.h | C++ |

---

## 🚀 Kurulum Yolları

### Yol 1: Otomatik Kurulum (Önerilen)
```powershell
# PowerShell'i yönetici olarak aç
.\dist\INSTALL.ps1 -InstallPath "C:\Program Files\RISVM" -AddToPath -CreateShortcuts
```

### Yol 2: El ile Kurulum
```powershell
# 1. Klasörleri oluştur
mkdir "C:\RISVM\bin"
mkdir "C:\RISVM\lib"
mkdir "C:\RISVM\config"

# 2. DLL'leri kopyala
Copy-Item "dist\dll\*.dll" "C:\RISVM\bin"

# 3. LIB'leri kopyala
Copy-Item "dist\lib\*.lib" "C:\RISVM\lib"

# 4. Config'i kopyala
Copy-Item "dist\config\*" "C:\RISVM\config"
```

### Yol 3: Proje İçine Entegrasyon
```bash
# SDK'yı projenin içine yerleştir
cp -r risvm/sdk your_project/external/risvm-sdk

# C# Project file'ında referansla
<Reference Include="risvm_core">
    <HintPath>external/risvm-sdk/dist/lib/risvm_core.lib</HintPath>
</Reference>
```

---

## 🛠️ Entegrasyon Kontrol Listesi

- [ ] `dist/ADD.md` oku
- [ ] Uygun örneği (`examples/`) seç
- [ ] Örneği kendi projeye uyarla
- [ ] `dist/config/risvm.default.json` yapılandır
- [ ] DLL'leri `bin/` klasörüne kopyala
- [ ] Test programı çalıştır
- [ ] Üretim yapılandırması yap
- [ ] Dağıtım için DLL'leri paketlendir

---

## 🔍 Dosya Açıklamaları

### dist/ADD.md
✅ **TÜRKÇE**  
Adım adım entegrasyon kılavuzu. Buradan başlayın!  
Konular: C#, C++, Windows Forms, Electron, Windows Service

### dist/INSTALL.ps1
PowerShell kurulum betiği. SDK'yı otomatik olarak sisteme kurar.

### dist/dll/
Çalışma zamanı DLL dosyaları. Bu dosyaları programın `bin/` klasörüne kopyala.

### dist/lib/
C++ projesi için linking kitaplıkları (`.lib` dosyaları).

### dist/include/
C/C++ header dosyaları. `#include` edebilirsiniz.

### examples/
Her dil için çalışan örnek projeler. Kopyalayıp çalıştırabilirsiniz.

### cmake/
CMake kullanıyorsan bu `XRTVMConfig.cmake` kullan.

---

## 🆘 Yaygın Sorular

**"Nereden başlamalıyım?"**  
→ `dist/ADD.md` oku, sonra `examples/CSharp_BasicIntegration/` çalıştır

**"DLL'i nereye kopyalamalıyım?"**  
→ Programın `.exe` dosyasının yanında bulunan `bin/` klasörü

**"RISVM başlatma başarısız oldu"**  
→ `dist/ADD.md` → Sorun Giderme bölümü

**"CPU çok yüksek kullanıyor"**  
→ `risvm.default.json`'da `protection.level` düşür

**"C++ kullanıyorum"**  
→ `examples/CPlusPlus_Advanced/` ve `cmake/XRTVMConfig.cmake` kullan

---

## 📞 Destek

- 📖 **Belgeler:** `.kiro/specs/root-ida-security-vm/` (ana belge)
- 💬 **Örnekler:** `examples/` (4 örnek kod)
- 🛠️ **Araçlar:** `dist/tools/xrtvm-cli.exe` (komut satırı)

---

**Sürüm:** 1.0.0-beta | **Tarih:** Temmuz 2026 | **Durum:** Geliştirme Aşamasında
