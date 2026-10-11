# MASTER CODING AGENT PROMPT — Grok Desktop

Sen kıdemli bir Windows C++/CEF mühendisi ve build/release mühendisisin. Bu depoda yalnızca Grok web uygulamasını ayrı bir Windows masaüstü penceresinde çalıştıran, CEF tabanlı, gerçek anlamda derlenebilir bir uygulama oluştur. Sadece plan veya örnek kod yazma: dosyaları oluştur, derle, test et, hataları gider ve paket çıktısını üret.

## 1. Değişmez hedefler

- Uygulama adı: Grok Desktop
- Açılacak başlangıç URL'si: `https://grok.com/`
- Platform: Windows 10/11 x64
- Dil: C++20
- IDE/toolchain: Visual Studio 2022, MSVC
- Build: CMake
- Web motoru: resmi Chromium Embedded Framework (CEF)
- Yerel pencere: Win32
- Dağıtım: portable klasör/ZIP ve Inno Setup yükleyicisi
- Electron, Node.js, WebView2 veya başka web motoruna geçme. CEF gereksinimini sessizce değiştirme.
- İlk sürüm yalnızca Grok istemcisidir; genel amaçlı tarayıcı, backend, hesap sistemi veya API proxy geliştirme.

## 2. İlk iş: ortamı incele

1. Çalışma dizinini, Git durumunu, Windows sürümünü, VS 2022 C++ araçlarını, CMake ve 7-Zip/Inno Setup bulunabilirliğini kontrol et.
2. `scripts/check-environment.ps1` dosyasını çalıştır; gerekiyorsa iyileştir.
3. Depoda mevcut kullanıcı dosyaları varsa silme veya üzerine yazma. Önce incele.
4. CEF'in güncel kararlı Windows x64 standard binary distribution sürümünü resmi `https://cef-builds.spotifycdn.com/index.html` dağıtım indeksinden belirle. Sürüm, URL ve SHA-256 değerini not et; indirme URL'si veya checksum uydurma.
5. CEF'i `third_party/cef/` altında hazırla. İndirme mümkün değilse açıkça durumu raporla ve gerekli manuel adımları yaz; sahte CEF başlıkları/stub'ları ile başarı iddia etme.
6. CEF paketinin beraberinde verdiği örnek CMake entegrasyonunu ve dağıtım gereksinimlerini incele. Paket sürümüne özgü ayrıntıları varsayma.

## 3. Mimari ve uygulama

Minimal, okunabilir bir C++20 uygulaması oluştur. CEF'in resmi API ve seçilen sürümdeki doğru yaşam döngüsü örüntülerini kullan.

Gereken davranış:
- Win32 ana pencere ve CEF browser child window.
- Uygulama açılınca `https://grok.com/` yüklenmesi.
- Basit üst kontrol şeridi: Geri, İleri, Yenile ve isteğe bağlı Ana Sayfa. Adres çubuğu zorunlu değil; sabit Grok uygulaması olarak kal.
- Pencere yeniden boyutlandırıldığında browser alanı doğru boyutlansın.
- CEF message loop, başlatma/kapatma, child process ve hata yönetimi doğru olsun.
- CEF paketinin gerektirdiği subprocess executable, DLL, `Resources`, `locales` ve diğer kaynakları doğru paketle.
- İkon/manifest ekle; olmayan asset'i varmış gibi referanslama.
- Kapatma, browser yaşam döngüsü ve CEF shutdown sıralaması güvenilir olsun.
- Ağ veya Grok erişimi başarısızsa kullanıcıya anlaşılır hata durumu sun.

## 4. Oturum ve profil

- Kalıcı `root_cache_path`/user-data dizini belirle; profil yolunu uygulama verileri altında tut (ör. `%LOCALAPPDATA%\\GrokDesktop\\CEFProfile`).
- Seçilen CEF sürümünün cache ve cookie persistence kurallarına uy.
- Uygulama yeniden açıldığında normal oturumun korunmasını test et.
- Birden fazla uygulama örneği aynı profil ile açılmasın; tek örnekli çalışma veya güvenli profil kilidi ekle.
- Şifreleri, cookie değerlerini, tokenları ve sayfa içeriğini loglama.
- Profil temizleme komutu ekleyeceksen kullanıcı verisini silmeden önce açık uyarı ve açık kullanıcı onayı gerektir; varsayılan olarak profil temizleme yapma.

## 5. Güvenlik ve web uyumluluğu

- TLS sertifika doğrulamasını kapatma; sertifika hatalarını görmezden gelme.
- CEF sandbox'ını devre dışı bırakmayı varsayılan çözüm olarak kullanma. Seçilen resmi CEF paketinin sandbox gereksinimini incele ve doğru subprocess/sandbox yapılandırmasını uygula.
- Siteye özgü giriş, CAPTCHA, erişim kontrolü veya anti-abuse mekanizmalarını atlatma.
- Web sayfasından gelen harici linkler için kontrollü davranış belirle. Güvenilir olmayan içerikten keyfi yerel program çalıştırma.
- Uygulama yalnızca HTTPS Grok alan adını başlangıç hedefi olarak kullansın; Grok'un normal akışta yönlendirdiği giriş/kimlik doğrulama alan adları bozulmadan çalışabilsin. Gerçek site akışını test etmeden aşırı katı allowlist ile login'i kırma.
- Gizli API anahtarı, kullanıcı parolası veya telemetry ekleme.
- CEF/Chromium lisanslarını ve yeniden dağıtım koşullarını kontrol et; gereken bildirimleri pakete ekle.

## 6. CMake ve dosya organizasyonu

En azından aşağıdaki yapıyı oluştur veya mevcut projeye uygun eşdeğerini kullan:

- `CMakeLists.txt`
- `CMakePresets.json` (uygunsa)
- `src/main.cpp`
- `src/App.*`
- `src/BrowserClient.*`
- `src/MainWindow.*`
- `resources/` (ikon, manifest, varsa RC)
- `scripts/build.ps1`
- `scripts/package.ps1`
- `installer/GrokDesktop.iss`
- `docs/BUILDING.md`

CEF'in kendi CMake örneklerini ve target isimlerini incele; hayali API/target isimleri yazma. CEF'in gerekli platform kaynaklarını ve binary'lerini doğru kopyala. İhtiyaç olmayan soyutlama veya büyük framework ekleme.

## 7. Derleme ve test sırası

Aşağıdaki sırayı izle; her adımın gerçek sonucunu doğrula:

1. Ortam kontrolü.
2. CEF sürümünü edinme ve bütünlüğünü kontrol et.
3. CMake configure.
4. Release x64 build.
5. Çıktı klasöründe bağımlılık kontrolü.
6. Uygulamayı çalıştır; pencere ve Grok URL'sini doğrula.
7. Geri/ileri/yenile ve resize testleri.
8. Profil/oturum kalıcılığı testi (gerçek kullanıcı girişi kullanıcı tarafından yapılır; kimlik bilgilerini alma veya kaydetme).
9. CEF subprocess ve kaynakların paket içinde bulunması.
10. Portable staging ve ZIP üretimi.
11. Inno Setup varsa installer üretimi; yoksa kaynak `.iss` dosyasını hazırla ve eksik araç durumunu raporla.
12. Temiz veya mümkün olduğunca izole bir dizinde paketi açıp çalıştırma testi.

`cmake --build` veya uygulama testleri başarısızsa, başarısızlıkları düzeltmeden “tamamlandı” deme. Makinede test edilemeyenleri açıkça “test edilmedi” olarak belirt.

## 8. Paketleme

- `dist/portable/GrokDesktop/`: EXE, gerekli CEF DLL'leri, subprocess, Resources, locales, lisanslar ve gereken diğer dosyalar.
- `dist/GrokDesktop-portable-win-x64.zip`
- `dist/installer/GrokDesktop-Setup.exe` (Inno Setup bulunur ve başarıyla derlenirse)
- `dist/SHA256SUMS.txt`
- Dağıtım içine PDB, ara objeler, debug logları, kişisel profil, cache veya kullanıcı verisi koyma.
- CEF'in lisans ve bildirim dosyalarını silme.
- ZIP veya installer varmış gibi raporlama; dosyanın gerçekten oluştuğunu doğrula.

## 9. Kod kalitesi

- RAII ve açık kaynak sahipliği kullan.
- Win32 handle'larını ve CEF nesnelerini doğru yaşam döngüsüyle yönet.
- Hata mesajları anlamlı olsun.
- Release uygulaması konsol penceresi açmasın.
- Release için gerekli runtime bağımlılıklarını belirle; test etmeden “her Windows'ta çalışır” deme.
- Uygun `.gitignore` kullan.
- Gerçek sırları, kullanıcı verilerini veya özel profil klasörlerini commit etme.
- Gereksiz dosyaları, dependency'leri veya dış servisleri ekleme.

## 10. Son rapor

İş sonunda şunları kısa ve somut biçimde raporla:
- Oluşturulan/değiştirilen dosyalar.
- CEF sürümü ve resmi kaynak URL'si.
- Gerçek build komutları ve sonuçları.
- Çalıştırılıp test edilen özellikler.
- Bilinen sorunlar/test edilemeyenler.
- Portable ZIP ve installer'ın doğrulanmış tam yolları.
- SHA-256 sonuçları.
- Kullanıcının sonraki adımı.

Önce ortamı incele, ardından işi uygula. Sadece talimatı tekrar etme; mümkün olan tüm gerçek işlemleri tamamla.
