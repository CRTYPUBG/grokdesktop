# Geliştirme Planı

## P0 — Ortam ve bağımlılık
- [ ] Windows/VS 2022/MSVC/CMake kontrolü
- [ ] Resmi CEF Windows x64 dağıtımını belirleme
- [ ] Kaynak URL'si ve SHA-256 doğrulaması
- [ ] Lisans ve redistribüsyon koşullarını kontrol etme

## P1 — Minimal CEF penceresi
- [ ] Win32 ana pencere
- [ ] CEF başlatma, browser client ve yaşam döngüsü
- [ ] `https://grok.com/` başlangıç URL'si
- [ ] Resize ve kapatma
- [ ] Subprocess, DLL ve resources

## P2 — Temel kullanım
- [ ] Geri/ileri/yenile
- [ ] Yükleme ve hata durumu
- [ ] Tek örnekli çalışma/profil kilidi
- [ ] Kalıcı profil ve cookie persistence

## P3 — Release
- [ ] CMake Release x64
- [ ] Konsolsuz uygulama
- [ ] DLL/resource dependency kontrolü
- [ ] Lisans bildirimleri

## P4 — Dağıtım
- [ ] Portable staging
- [ ] Portable ZIP
- [ ] SHA-256 manifesti
- [ ] Inno Setup installer (araç mevcutsa)
- [ ] Temiz dizinde açma ve çalıştırma testi

## Done definition
“Tamamlandı” demek için gerçek Release build başarılı olmalı; gerekli CEF dosyaları paketlenmeli; uygulama başlatılmalı; test sonucu ve paket yolları doğrulanmalıdır. Makinede yapılamayan testler açıkça belirtilmelidir.
