# Test Planı

## Ortam
- [ ] VS 2022 C++ workload mevcut
- [ ] CMake ve Windows SDK mevcut
- [ ] CEF arşivi resmi kaynaktan indirildi
- [ ] Hash/integrity kontrolü yapıldı

## Uygulama
- [ ] Release x64 build başarılı
- [ ] `GrokDesktop.exe` başlıyor ve konsol göstermiyor
- [ ] Grok sayfası yükleniyor
- [ ] Browser alanı pencere boyutuna uyuyor
- [ ] Geri/ileri/yenile çalışıyor
- [ ] Pencere kapatma sırasında crash/hang yok
- [ ] Ağ hatası kullanıcıya anlaşılır gösteriliyor

## Profil
- [ ] Profil `%LOCALAPPDATA%` altında
- [ ] Yeniden başlatmada oturum kalıcılığı test edildi
- [ ] Profil/cache dağıtım paketine dahil değil
- [ ] Cookie/token içerikleri loglanmıyor

## Paket
- [ ] CEF subprocess, DLL, resources ve locales mevcut
- [ ] Gerekli lisans bildirimleri dahil
- [ ] Portable ZIP açılıp çalıştırıldı
- [ ] Installer derlendi ve test edildi (araç varsa)
- [ ] SHA-256 manifesti gerçek dosyalar üzerinden üretildi

Her test için gerçek sonuç ve varsa hata notu kaydedilmelidir.
