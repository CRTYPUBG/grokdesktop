using System;
using System.IO;
using System.Runtime.InteropServices;

namespace RISVMIntegration
{
    /// <summary>
    /// RISVM Temel Entegrasyon Örneği
    /// 
    /// Bu örnek, bir C# uygulamasına RISVM koruma sistemini nasıl ekleyeceğinizi göstermektedir.
    /// 
    /// Gereksinimler:
    /// - risvm_core.dll (bin klasöründe)
    /// - risvm.default.json (programın çalışma dizininde)
    /// </summary>
    class Program
    {
        // ==================== DLL İçe Aktarımları ====================
        
        [DllImport("risvm_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int risvm_init(string config_json);

        [DllImport("risvm_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr risvm_get_status();

        [DllImport("risvm_core.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool risvm_feature_enabled(string feature_id);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern void SetConsoleCP(uint wCodePageID);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern void SetConsoleOutputCP(uint wCodePageID);

        // ==================== Ana Program ====================

        static void Main(string[] args)
        {
            // UTF-8 desteği
            SetConsoleCP(65001);
            SetConsoleOutputCP(65001);
            Console.OutputEncoding = System.Text.Encoding.UTF8;

            PrintBanner();

            try
            {
                // Adım 1: Yapılandırma yükle
                Console.WriteLine("\n[Adım 1] Yapılandırma dosyası yükleniyor...");
                string config = LoadConfiguration();

                // Adım 2: RISVM başlat
                Console.WriteLine("[Adım 2] RISVM başlatılıyor...");
                int initResult = InitializeRISVM(config);

                if (initResult != 0)
                {
                    Console.WriteLine("❌ RISVM başlatma hatası!", Console.ForegroundColor);
                    return;
                }

                // Adım 3: Durum kontrol
                Console.WriteLine("[Adım 3] Koruma durumu kontrol ediliyor...");
                string status = GetStatus();

                // Adım 4: Özellik kontrolü
                Console.WriteLine("[Adım 4] Özellik durumları kontrol ediliyor...");
                CheckFeatures();

                // Adım 5: Korunan işlem
                Console.WriteLine("[Adım 5] Korunan işlem başlatılıyor...");
                PerformProtectedOperation();

                PrintSuccessSummary();
            }
            catch (DllNotFoundException ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"\n❌ DLL Hatası: {ex.Message}");
                Console.WriteLine("   → risvm_core.dll'in bin dizininde olduğundan emin olun");
                Console.ResetColor();
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"\n❌ Hata: {ex.Message}");
                Console.ResetColor();
            }

            Console.WriteLine("\nÇıkmak için bir tuşa basın...");
            Console.ReadKey();
        }

        // ==================== Yardımcı Metotlar ====================

        static void PrintBanner()
        {
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine("╔═══════════════════════════════════════════════════════╗");
            Console.WriteLine("║     RISVM Security Platform - C# Entegrasyon Örneği  ║");
            Console.WriteLine("╚═══════════════════════════════════════════════════════╝");
            Console.ResetColor();
        }

        static string LoadConfiguration()
        {
            const string configPath = "risvm.default.json";

            if (!File.Exists(configPath))
            {
                // Varsayılan yapılandırma oluştur
                string defaultConfig = @"{
  ""protection"": {
    ""level"": ""High"",
    ""crypto_method"": ""AES256_GCM"",
    ""key_derivation"": ""hardware"",
    ""integrity_check_interval_ms"": 5000,
    ""anti_debug"": true,
    ""anti_reverse"": true,
    ""vm_detection"": true,
    ""sandbox_detection"": true
  },
  ""response"": {
    ""threat_escalation"": true,
    ""max_threat_score"": 100,
    ""session_kill_on_critical"": true
  }
}";
                File.WriteAllText(configPath, defaultConfig);
                Console.WriteLine($"   ℹ Yapılandırma dosyası oluşturuldu: {configPath}");
            }

            string config = File.ReadAllText(configPath);
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("   ✓ Yapılandırma başarıyla yüklendi");
            Console.ResetColor();
            return config;
        }

        static int InitializeRISVM(string config)
        {
            try
            {
                int result = risvm_init(config);
                
                if (result == 0)
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine("   ✓ RISVM başarıyla başlatıldı");
                    Console.ResetColor();
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine($"   ❌ RISVM başlatma hatası: kod {result}");
                    Console.ResetColor();
                }

                return result;
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine($"   ❌ Başlatma sırasında hata: {ex.Message}");
                Console.ResetColor();
                return -1;
            }
        }

        static string GetStatus()
        {
            try
            {
                IntPtr statusPtr = risvm_get_status();
                string status = Marshal.PtrToStringAnsi(statusPtr);
                
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine($"   ✓ Koruma Durumu: {status}");
                Console.ResetColor();

                return status;
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine($"   ⚠ Durum alınamadı: {ex.Message}");
                Console.ResetColor();
                return "Unknown";
            }
        }

        static void CheckFeatures()
        {
            string[] features = { "premium", "advanced_logging", "anti_tamper" };

            foreach (string feature in features)
            {
                try
                {
                    bool enabled = risvm_feature_enabled(feature);
                    Console.ForegroundColor = enabled ? ConsoleColor.Green : ConsoleColor.Yellow;
                    Console.WriteLine($"   {(enabled ? "✓" : "○")} {feature}: {(enabled ? "Etkin" : "Devre Dışı")}");
                    Console.ResetColor();
                }
                catch (Exception ex)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine($"   ❌ {feature}: Kontrol hatası - {ex.Message}");
                    Console.ResetColor();
                }
            }
        }

        static void PerformProtectedOperation()
        {
            Console.WriteLine("\n╔═══════════════════════════════════════════════════════╗");
            Console.WriteLine("║          KORUNAN İŞLEM BAŞLATILIYOR                  ║");
            Console.WriteLine("╚═══════════════════════════════════════════════════════╝");

            // Simüle edilmiş hassas işlem
            for (int i = 0; i < 5; i++)
            {
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine($"  🔄 İşlem adım {i + 1}/5 devam ediyor...");
                Console.ResetColor();
                System.Threading.Thread.Sleep(500);
            }

            Console.WriteLine("\n╔═══════════════════════════════════════════════════════╗");
            Console.WriteLine("║          KORUNAN İŞLEM TAMAMLANDI                    ║");
            Console.WriteLine("╚═══════════════════════════════════════════════════════╝");
        }

        static void PrintSuccessSummary()
        {
            Console.WriteLine("\n");
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("╔═══════════════════════════════════════════════════════╗");
            Console.WriteLine("║              ✓ Tüm Adımlar Başarılı!                ║");
            Console.WriteLine("╚═══════════════════════════════════════════════════════╝");
            Console.ResetColor();

            Console.WriteLine("\n📋 Özet:");
            Console.WriteLine("   • RISVM başarıyla başlatıldı");
            Console.WriteLine("   • Tüm koruma mekanizmaları etkin");
            Console.WriteLine("   • Korunan işlem başarıyla tamamlandı");
            Console.WriteLine("\n💡 İpuçları:");
            Console.WriteLine("   1. risvm.default.json'ı özelleştir");
            Console.WriteLine("   2. ADD.md'deki diğer entegrasyon yöntemlerini incele");
            Console.WriteLine("   3. Hata işleme logic'ini projen için uyarla");
        }
    }
}
