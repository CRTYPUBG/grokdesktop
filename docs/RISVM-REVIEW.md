# RISVM SDK — Yerel İnceleme Notları

Bu belge, `third_party/RISVM-sdk/` ve `third_party/Include/` altındaki dosyaların
kodlama ajanı tarafından **gerçekten okunması** için hazırlanmıştır. Aşağıdaki
bulgular bu depodaki dosyaların incelenmesine dayanır; dış kaynaklardan
varsayım yapılmamıştır.

## Okunması gereken yollar

| Yol | Amaç |
|-----|------|
| `third_party/RISVM-sdk/README.md` | SDK özeti (İngilizce) |
| `third_party/RISVM-sdk/INDEX.md` | Dosya haritası (Türkçe) |
| `third_party/RISVM-sdk/DISTRIBUTION_MANIFEST.md` | Dağıtım manifestosu |
| `third_party/RISVM-sdk/dist/ADD.md` | Entegrasyon kılavuzu (Türkçe) |
| `third_party/RISVM-sdk/dist/INSTALL.ps1` | Kurulum betiği |
| `third_party/RISVM-sdk/compiler/include/xrtvm_compiler.h` | Derleyici API |
| `third_party/RISVM-sdk/runtime/include/xrtvm_runtime.h` | Runtime API |
| `third_party/RISVM-sdk/cmake/XRTVMConfig.cmake` | CMake entegrasyonu |
| `third_party/RISVM-sdk/vs-extension/` | VS uzantısı kaynakları |
| `third_party/RISVM-sdk/examples/` | Kod örnekleri |
| `third_party/Include/ThemidaSDK/` | Themida SDK başlıkları + lib |
| `third_party/Include/VmProtectSDK/` | VMProtect SDK (Rust bind + lib/dll) |

## Bulgular (bu depo üzerinden)

1. **RISVM-sdk dağıtım paketi eksik.** `DISTRIBUTION_MANIFEST.md` ve `README.md`
   `dist/dll/risvm_core.dll`, `risvm_crypto.dll`, `risvm_electron.dll`,
   `watchdog.exe`, `dist/lib/*.lib`, `dist/include/risvm_core.h`,
   `dist/config/*.json` ve `dist/tools/xrtvm-cli.exe` dosyalarını vaat ediyor.
   Depoda bunların **hiçbiri yok**; `dist/` altında yalnızca Markdown/txt betik
   dosyaları ve `INSTALL.ps1` bulunuyor. `compiler/include/` ve
   `runtime/include/` altında yalnızca birer header var; binary yok.
   **Sonuç: bu SDK bu haliyle derleme/link adımına dâhil edilemez.**

2. **Lisans dosyası yok.** `README.md` “See LICENSE file in the main
   repository.” diyor; ne `RISVM-sdk/` altında ne de depo kökünde bir LICENSE
   dosyası var. Dağıtım ve yeniden dağıtım izni belirsizdir.

3. **Header API’leri incelemeye değer, entegrasyon değil.** `xrtvm_compiler.h`
   ve `xrtvm_runtime.h` tek başına çağrılabilir bir koruma katmanı sunmuyor;
   gerçek motorun binary’si pakette olmadığı için bunları Grok Desktop’a
   bağlamak mümkün değil.

4. **`third_party/Include/` içinde üçüncü parti koruma ürünleri var:**
   - `ThemidaSDK/` — WinLicense/Themida başlıkları + `SecureEngineSDK32/64.lib`
   - `VmProtectSDK/` — VMProtect SDK (Rust binding + `VMProtectSDK64.dll/.lib`)
   
   Bunlar **ticari** ürünlerin SDK’larıdır. Kendi lisans koşulları vardır
   (genellikle ürün başına lisans). Bu depoda lisans metinleri yok.
   Grok Desktop derlemesine **şu an dâhil değiller** (CMakeLists yalnızca CEF
   ve uygulama kaynaklarını derler); yalnızca `third_party` altında duruyorlar.

5. **Grok Desktop ile şu anki entegrasyon durumu: yok.** `src/CMakeLists.txt`
   RISVM’e, Themida’ya veya VMProtect’a link etmiyor. Uygulama CEF + Win32
   olarak kalır. Korumalı dağıtım istenirse önce:
   - geçerli lisans metinleri ve tam DLL/LIB paketi temin edilmeli,
   - `docs/BUILDING.md`’e “Koruma katmanı (isteğe bağlı)” adımı eklenmeli,
   - antivirüs false-positive riski ve sandbox etkileşimi test edilmeli.

## Kodlama ajanı için kontrol listesi

- [ ] `README.md`, `INDEX.md`, `DISTRIBUTION_MANIFEST.md` okundu (yukarıdaki bulguları doğrula).
- [ ] `dist/ADD.md` ve `INSTALL.ps1` okundu; binary eksikliğini doğrula.
- [ ] `compiler/include/xrtvm_compiler.h` ve `runtime/include/xrtvm_runtime.h` tarandı.
- [ ] `third_party/Include/ThemidaSDK/ThemidaSDK.h` tarandı (ticari SDK).
- [ ] `third_party/Include/VmProtectSDK/README.md` ve `src/lib.rs` tarandı.
- [ ] Lisans metni bulunamazsa RISVM/Themida/VMProtect’ı dağıtıma **ekleme**.
- [ ] Grok Desktop kod tabanına koruma katmanı **eklemek istenmedikçe** CMake’i değiştirme.

## Karar (mevcut durum)

- RISVM SDK: **belge + eksik paket** — entegre edilmiyor.
- Themida/VMProtect: **yalnızca `third_party` altında duruyor** — derlemeye
  bağlı değil, lisans doğrulanana kadar dağıtıma dâhil edilmiyor.
- Güvenlik: uygulama TLS’i bypass etmez, parola/çerez loglamaz; koruma katmanı
  eklenirse bu kurallar korunmalıdır.
