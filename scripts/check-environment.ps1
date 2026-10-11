$ErrorActionPreference = "Continue"

Write-Host "=== Grok Desktop build environment check ===" -ForegroundColor Cyan
Write-Host "OS: $([System.Environment]::OSVersion.VersionString)"
Write-Host "Architecture: $env:PROCESSOR_ARCHITECTURE"
Write-Host ""

$commands = @("cmake", "git", "cl", "devenv", "iscc", "7z")
foreach ($name in $commands) {
    $cmd = Get-Command $name -ErrorAction SilentlyContinue
    if ($cmd) {
        Write-Host "[FOUND] $name : $($cmd.Source)" -ForegroundColor Green
    } else {
        Write-Host "[MISSING/NOT IN PATH] $name" -ForegroundColor Yellow
    }
}

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    Write-Host "`nVisual Studio installations:"
    & $vswhere -all -products * -format table
} else {
    Write-Host "`n[INFO] vswhere.exe not found at standard location." -ForegroundColor Yellow
}

Write-Host "`nCEF folder:"
if (Test-Path ".\third_party\cef") {
    Get-ChildItem ".\third_party\cef" -Force | Select-Object -First 12 Name, Mode
} else {
    Write-Host "[NOT READY] third_party\cef is not present yet." -ForegroundColor Yellow
}

Write-Host "`nThis script checks presence only; it does not claim that a build will succeed."
