# Builds Grok Desktop (Release x64) with CMake + Visual Studio 2022.
# Usage:
#   .\scripts\build.ps1                 # configure + build (sandbox ON)
#   .\scripts\build.ps1 -Clean          # delete build dir first
#   .\scripts\build.ps1 -NoSandbox      # build without the CEF sandbox
#   .\scripts\build.ps1 -Target install # run the install target after build

[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$NoSandbox,
    [string]$Target = "",
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

# --- Locate VS via vswhere -------------------------------------------------
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe not found. Install Visual Studio 2022 with the C++ desktop workload."
}
$vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsPath) {
    throw "No Visual Studio installation with MSVC x64 tools found."
}
$cmakePath = Join-Path $vsPath "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if (-not (Test-Path $cmakePath)) {
    # Fall back to a system cmake if the VS-bundled one is missing.
    $cmakePath = (Get-Command cmake -ErrorAction SilentlyContinue).Source
    if (-not $cmakePath) { throw "cmake.exe not found (neither VS-bundled nor on PATH)." }
}

# --- Verify CEF is present -------------------------------------------------
$cefDir = Join-Path $RepoRoot "third_party\cef"
if (-not (Test-Path (Join-Path $cefDir "CMakeLists.txt"))) {
    throw "CEF not found at third_party\cef. Extract the official CEF binary distribution there first."
}

# --- Configure -------------------------------------------------------------
$buildDir = if ($NoSandbox) { Join-Path $RepoRoot "build-nosandbox" } else { Join-Path $RepoRoot "build" }
if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "Cleaning $buildDir ..."
    Remove-Item $buildDir -Recurse -Force
}

$useSandbox = if ($NoSandbox) { "OFF" } else { "ON" }
Write-Host "Configuring ($buildDir, USE_SANDBOX=$useSandbox) ..." -ForegroundColor Cyan
& $cmakePath -S $RepoRoot -B $buildDir -G "Visual Studio 17 2022" -A x64 `
    -DUSE_SANDBOX:BOOL=$useSandbox
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed (exit $LASTEXITCODE)." }

# --- Build -----------------------------------------------------------------
Write-Host "Building $Config ..." -ForegroundColor Cyan
if ($Target) {
    & $cmakePath --build $buildDir --config $Config --target $Target -- /m
} else {
    & $cmakePath --build $buildDir --config $Config -- /m
}
if ($LASTEXITCODE -ne 0) { throw "Build failed (exit $LASTEXITCODE)." }

$outDir = Join-Path $buildDir "src\$Config"
$exe = Join-Path $outDir "GrokDesktop.exe"
if (Test-Path $exe) {
    # In sandbox mode the launcher is CEF's stock bootstrap.exe; inject our
    # .ico so the taskbar / title bar show the Grok Desktop logo.
    $ico = Join-Path $RepoRoot "resources\app.ico"
    if (Test-Path $ico) {
        try {
            & (Join-Path $PSScriptRoot "embed-icon.ps1") -ExePath $exe -IcoPath $ico
        } catch {
            Write-Warning "Icon embed failed (non-fatal): $_"
        }
    }
    Write-Host "`nBuild succeeded: $exe" -ForegroundColor Green
} else {
    Write-Host "`nBuild finished but GrokDesktop.exe was not found in $outDir" -ForegroundColor Yellow
}
