# Packages the Release build into a portable ZIP and an Inno Setup installer.
# Usage:
#   .\scripts\package.ps1                     # portable ZIP + installer
#   .\scripts\package.ps1 -PortableOnly       # ZIP only
#   .\scripts\package.ps1 -InstallerOnly      # installer only
#   .\scripts\package.ps1 -BuildDir <path>    # package a specific build dir

[CmdletBinding()]
param(
    [string]$BuildDir = "",
    [switch]$PortableOnly,
    [switch]$InstallerOnly
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

# --- Locate the Release output --------------------------------------------
if (-not $BuildDir) {
    $candidates = @(
        (Join-Path $RepoRoot "build\src\Release"),
        (Join-Path $RepoRoot "build-nosandbox\src\Release")
    )
    foreach ($c in $candidates) {
        if (Test-Path (Join-Path $c "GrokDesktop.exe")) { $BuildDir = $c; break }
    }
}
if (-not $BuildDir -or -not (Test-Path (Join-Path $BuildDir "GrokDesktop.exe"))) {
    throw "No built GrokDesktop.exe found. Run scripts\build.ps1 first."
}
Write-Host "Packaging from: $BuildDir" -ForegroundColor Cyan

$Version = "1.0.0"
$DistDir = Join-Path $RepoRoot "dist"
$PortableDir = Join-Path $DistDir "portable\GrokDesktop"
$InstallerDir = Join-Path $DistDir "installer"

# --- Collect files to ship -------------------------------------------------
# Everything in the build output except MSVC import artifacts (.exp/.lib).
$files = Get-ChildItem $BuildDir -Recurse -File |
    Where-Object {
        -not ($_.Name.EndsWith(".exp") -or $_.Name.EndsWith(".lib") -or
              $_.Name -eq "grok_popup_log.txt")
    }

$totalMB = [math]::Round(($files | Measure-Object Length -Sum).Sum / 1MB, 1)
Write-Host "Files to package: $($files.Count) ($totalMB MB)"

# --- Portable ZIP ----------------------------------------------------------
if (-not $InstallerOnly) {
    if (Test-Path $PortableDir) { Remove-Item $PortableDir -Recurse -Force }
    New-Item -ItemType Directory -Path $PortableDir -Force | Out-Null
    foreach ($f in $files) {
        $rel = $f.FullName.Substring($BuildDir.Length).TrimStart("\", "/")
        $dest = Join-Path $PortableDir $rel
        $destDir = Split-Path -Parent $dest
        if (-not (Test-Path $destDir)) { New-Item -ItemType Directory -Path $destDir -Force | Out-Null }
        Copy-Item $f.FullName $dest -Force
    }

    # Ship a short README with the portable build.
    @"
Grok Desktop $Version (portable)
================================

1. Extract this archive to any folder.
2. Run GrokDesktop.exe.
3. User profile data is stored in %LOCALAPPDATA%\GrokDesktop (not next to
   the exe), so the portable folder can be moved or deleted without losing
   your Grok login.

This build opens https://grok.com/ in a native Windows window.
"@ | Set-Content (Join-Path $PortableDir "PORTABLE-README.txt") -Encoding UTF8

    New-Item -ItemType Directory -Path $DistDir -Force | Out-Null
    $zipPath = Join-Path $DistDir "GrokDesktop-$Version-win64-portable.zip"
    if (Test-Path $zipPath) { Remove-Item $zipPath -Force }

    $sevenZip = "${env:ProgramFiles}\7-Zip\7z.exe"
    if (Test-Path $sevenZip) {
        & $sevenZip a -tzip -mx=9 $zipPath (Join-Path $PortableDir "*") | Out-Null
    } else {
        Compress-Archive -Path (Join-Path $PortableDir "*") -DestinationPath $zipPath -CompressionLevel Optimal
    }
    if (-not (Test-Path $zipPath)) { throw "ZIP creation failed." }
    Write-Host "Portable ZIP: $zipPath" -ForegroundColor Green
}

# --- SHA-256 manifest ------------------------------------------------------
if (-not $InstallerOnly) {
    $zipPath = Join-Path $DistDir "GrokDesktop-$Version-win64-portable.zip"
    $hashes = @()
    if (Test-Path $zipPath) {
        $h = (Get-FileHash $zipPath -Algorithm SHA256).Hash.ToLower()
        $name = Split-Path -Leaf $zipPath
        $hashes += "$h  $name"
    }
    $issPath = Join-Path $InstallerDir "GrokDesktop-$Version-win64-setup.exe"
    if (Test-Path $issPath) {
        $h = (Get-FileHash $issPath -Algorithm SHA256).Hash.ToLower()
        $name = Split-Path -Leaf $issPath
        $hashes += "$h  $name"
    }
    if ($hashes.Count -gt 0) {
        $hashes | Set-Content (Join-Path $DistDir "SHA256SUMS.txt") -Encoding ASCII
        Write-Host "SHA256SUMS.txt written." -ForegroundColor Green
        $hashes | ForEach-Object { Write-Host "  $_" }
    }
}

# --- Inno Setup installer --------------------------------------------------
if (-not $PortableOnly) {
    $iscc = "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe"
    if (-not (Test-Path $iscc)) {
        Write-Host "[SKIP] Inno Setup Compiler not found at $iscc" -ForegroundColor Yellow
        return
    }
    New-Item -ItemType Directory -Path $InstallerDir -Force | Out-Null
    $iss = Join-Path $RepoRoot "installer\GrokDesktop.iss"
    if (-not (Test-Path $iss)) { throw "Installer script not found: $iss" }

    & $iscc "/DMyAppVersion=$Version" "/DSourceDir=$BuildDir" "/O$InstallerDir" $iss
    if ($LASTEXITCODE -ne 0) { throw "Inno Setup compilation failed (exit $LASTEXITCODE)." }

    $setup = Join-Path $InstallerDir "GrokDesktop-$Version-win64-setup.exe"
    if (Test-Path $setup) {
        Write-Host "Installer: $setup" -ForegroundColor Green
        $h = (Get-FileHash $setup -Algorithm SHA256).Hash.ToLower()
        $hashes = @()
        $zipPath = Join-Path $DistDir "GrokDesktop-$Version-win64-portable.zip"
        if (Test-Path $zipPath) {
            $hashes += "$((Get-FileHash $zipPath -Algorithm SHA256).Hash.ToLower())  $(Split-Path -Leaf $zipPath)"
        }
        $hashes += "$h  $(Split-Path -Leaf $setup)"
        $hashes | Set-Content (Join-Path $DistDir "SHA256SUMS.txt") -Encoding ASCII
        Write-Host "SHA256SUMS.txt updated with installer hash." -ForegroundColor Green
        $hashes | ForEach-Object { Write-Host "  $_" }
    }
}
