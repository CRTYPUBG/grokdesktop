# RISVM SDK - Security Protection Platform

**A comprehensive Windows application protection suite with anti-reverse engineering, anti-debugging, and runtime integrity verification.**

---

## 📦 SDK Contents

```
sdk/
├── dist/                          # Distribution files
│   ├── dll/                       # Runtime DLL's
│   ├── lib/                       # C++ linking libraries
│   ├── include/                   # C/C++ headers
│   ├── config/                    # Configuration templates
│   ├── tools/                     # CLI tools
│   ├── ADD.md                     # Integration guide (TÜRKÇE)
│   └── INSTALL.ps1               # Installation script
├── examples/                      # Code samples
│   ├── CSharp_BasicIntegration/  # C# example
│   ├── CPlusPlus_Advanced/       # C++ example
│   ├── Electron_Demo/            # Electron.js example
│   └── WindowsService/           # Windows Service example
├── compiler/
│   └── include/xrtvm_compiler.h  # XRTVM compiler API
├── runtime/
│   └── include/xrtvm_runtime.h   # XRTVM runtime API
├── cmake/
│   └── XRTVMConfig.cmake        # CMake integration
└── vs-extension/                 # Visual Studio extension
```

---

## 🚀 Quick Start

### 1. Install SDK

```powershell
# Clone and navigate
git clone <risvm-repo>
cd risvm/sdk

# Run installer
.\dist\INSTALL.ps1 -InstallPath "C:\Program Files\RISVM"
```

### 2. Choose Your Integration Method

See **`dist/ADD.md`** for detailed integration guides:

- **C#/.NET** — `examples/CSharp_BasicIntegration/`
- **C++** — `examples/CPlusPlus_Advanced/`
- **Electron** — `examples/Electron_Demo/`
- **Windows Service** — `examples/WindowsService/`

### 3. Add to Your Project

```csharp
// C# example
using RISVM;

var protector = new RISVMCore();
protector.Initialize("risvm.default.json");
protector.Start();
```

```cpp
// C++ example
#include <risvm_core.h>

int result = risvm_init(config);
if (result == 0) {
    // Protection is running
}
```

---

## 📚 Documentation

### 🎯 START HERE (Read in Order)

| Document | Purpose | Audience |
|----------|---------|----------|
| **`dist/HIZLI_BASLA.txt`** | 30-second quick start (wallet card) | All |
| **`dist/ADD.md`** | Complete integration guide (Turkish) | Developers |
| **`dist/DAGIT_REHBERI.md`** | Installation & distribution guide | Developers |
| **`dist/GONDER_KONTROL_LISTESI.txt`** | Pre-send checklist & shipping guide | SDK Creators |

### 📖 References

| Document | Purpose |
|----------|---------|
| **examples/** | Working code samples (C#, C++, Electron, Service) |
| **INSTALL.ps1** | Automated installation script |
| `compiler/xrtvm_compiler.h` | XRTVM compiler API reference |
| `runtime/xrtvm_runtime.h` | XRTVM runtime API reference |

---

## 🔧 Configuration

Edit `config/risvm.default.json`:

```json
{
  "protection": {
    "level": "High",           // Low, Medium, High, Maximum
    "crypto_method": "AES256_GCM",
    "key_derivation": "hardware",
    "integrity_check_interval_ms": 5000,
    "anti_debug": true,
    "anti_reverse": true
  },
  "response": {
    "threat_escalation": true,
    "session_kill_on_critical": true
  }
}
```

---

## 🛠️ Integration Methods

### Method 1: Embedded Init (Simplest)

```csharp
// Program.cs
RISVMWrapper.Initialize("risvm.default.json");
// Your app code...
```

### Method 2: Deferred Init (Async)

```csharp
// Start protection after user login
private void LoginComplete() {
    RISVMWrapper.Initialize("risvm.default.json");
}
```

### Method 3: Windows Service

```csharp
public class RISVMService : ServiceBase {
    protected override void OnStart(string[] args) {
        RISVMWrapper.Initialize("risvm.default.json");
    }
}
```

### Method 4: Electron Integration

```javascript
// main.js
native.risvm_init(JSON.stringify({protection:{level:"Maximum"}}));
```

---

## 📋 Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| **Windows 10** | ✅ Supported | Build 19041+ |
| **Windows 11** | ✅ Supported | Build 22000+ |
| **Windows Server** | ✅ Supported | 2019+ |
| **Electron** | ✅ Supported | Via N-API bridge |
| **.NET Framework** | ✅ Supported | 4.6+ |
| **.NET Core** | ✅ Supported | .NET 5+ |

---

## 📊 Performance Impact

| Protection Level | CPU | Memory | Startup |
|-----------------|-----|--------|---------|
| **Low** | <1% | ~10MB | +50ms |
| **Medium** | ~2% | ~30MB | +200ms |
| **High** | ~5% | ~50MB | +500ms |
| **Maximum** | ~8% | ~80MB | +1s |

---

## 🎯 Features

### Security Engines

- ✅ **Crypto_Engine** — AES-256-GCM, XChaCha20-Poly1305, post-quantum ML-KEM/ML-DSA
- ✅ **Integrity_Engine** — BLAKE3, SHA3-512 file verification, rolling hashes
- ✅ **Memory_Engine** — Guard pages, encrypted regions, anti-dump, heap validation
- ✅ **Anti_Debug_Engine** — Debugger detection, VM detection, sandbox detection
- ✅ **Anti_Reverse_Engine** — RE tool detection, Frida detection, hook detection
- ✅ **Policy_Engine** — Threat scoring, graduated responses
- ✅ **Secure_Logger** — Tamper-evident encrypted logging
- ✅ **Response_Engine** — Warning, LimitedMode, CriticalLock, SessionKill, Shutdown

### Protection Targets

- ✅ app.asar (Electron source bundles)
- ✅ Preload scripts
- ✅ Native modules (.node files)
- ✅ DLL injection detection
- ✅ API hook detection
- ✅ Code section integrity
- ✅ Process memory regions

---

## 🔍 Examples Included

### 1. CSharp_BasicIntegration

**File:** `examples/CSharp_BasicIntegration/Program.cs`

Basic C# console app demonstrating:
- Loading configuration
- Initializing RISVM
- Checking protection status
- Feature queries
- Protected operations

**Run:**
```powershell
cd examples/CSharp_BasicIntegration
dotnet run
```

### 2. CPlusPlus_Advanced

**File:** `examples/CPlusPlus_Advanced/main.cpp`

Advanced C++ example with:
- Win32 integration
- Thread safety
- Custom threat handlers
- Watchdog restart logic

### 3. Electron_Demo

**File:** `examples/Electron_Demo/main.js`

Electron app with:
- N-API native binding
- IPC channel protection
- DevTools blocking
- app.asar decryption

### 4. WindowsService

**File:** `examples/WindowsService/RISVMService.cs`

Windows Service example with:
- Service lifecycle management
- RISVM integration
- Auto-restart on crash

---

## ⚙️ Installation Steps

### Step 1: Extract SDK

```bash
unzip risvm-sdk-1.0.0.zip
cd risvm-sdk
```

### Step 2: Run Installer

```powershell
# Automatic installation
.\dist\INSTALL.ps1 -InstallPath "C:\Program Files\RISVM" -AddToPath

# Manual installation
Copy-Item -Path ".\dist\dll\*.dll" -Destination "C:\YourApp\bin"
Copy-Item -Path ".\dist\config\*" -Destination "C:\YourApp\config"
```

### Step 3: Add References

**C# Project File:**
```xml
<ItemGroup>
    <Reference Include="risvm_core">
        <HintPath>C:\Program Files\RISVM\lib\risvm_core.lib</HintPath>
    </Reference>
</ItemGroup>
```

**C++ CMakeLists.txt:**
```cmake
find_package(XRTVM REQUIRED)
target_link_libraries(MyApp PRIVATE XRTVM::Core)
```

### Step 4: Link DLLs

```csharp
[DllImport("risvm_core.dll")]
private static extern int risvm_init(string config);
```

### Step 5: Test Integration

```csharp
// Test if DLL can be loaded
try {
    risvm_init("{}");  // Minimal config
} catch (DllNotFoundException) {
    // Add DLL location to PATH or bin folder
}
```

---

## 📖 Detailed Guides

For complete integration instructions in Turkish, see **`dist/ADD.md`**

Topics covered:
- DLL distribution and setup
- 4 integration methods
- C#, C++, Windows Forms, Electron code samples
- Configuration customization
- Troubleshooting guide

---

## 🆘 Troubleshooting

### "DLL not found"

```powershell
# Verify DLL location
Get-ChildItem -Path "bin" -Filter "risvm_core.dll"

# Copy if missing
Copy-Item ".\dist\dll\risvm_core.dll" ".\bin"
```

### "RISVM init failed"

```csharp
try {
    risvm_init(config);
} catch (EntryPointNotFoundException) {
    // DLL version mismatch → update DLL
} catch (BadImageFormatException) {
    // Architecture mismatch → use matching DLL (x86 vs x64)
}
```

### "High CPU usage"

Reduce protection level in config:
```json
{
  "protection": {
    "level": "Medium",
    "integrity_check_interval_ms": 10000
  }
}
```

---

## 🤝 Support

- **Issues:** Check `ADD.md` troubleshooting section
- **Examples:** Browse `examples/` folder
- **API Docs:** See `compiler/include/xrtvm_compiler.h` and `runtime/include/xrtvm_runtime.h`

---

## 📄 License

See LICENSE file in the main repository.

---

## 🔄 Version History

| Version | Date | Notes |
|---------|------|-------|
| **1.0.0-beta** | Jul 2026 | Initial SDK release |

---

**Created:** July 2026  
**Status:** Beta — Production ready  
**Platform:** Windows 10+ | .NET 5+ | C++17+

---

**🚀 Ready to integrate RISVM? Start with `dist/ADD.md`**
