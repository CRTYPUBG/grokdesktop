# Embeds resources/app.ico into an existing PE executable (e.g. the CEF
# bootstrap copy of GrokDesktop.exe) using Win32 BeginUpdateResource APIs.
# Usage: .\embed-icon.ps1 -ExePath path\to\GrokDesktop.exe -IcoPath path\to\app.ico
param(
    [Parameter(Mandatory = $true)][string]$ExePath,
    [Parameter(Mandatory = $true)][string]$IcoPath
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $ExePath)) { throw "Exe not found: $ExePath" }
if (-not (Test-Path -LiteralPath $IcoPath)) { throw "Ico not found: $IcoPath" }

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public static class IconUpdater {
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    public static extern IntPtr BeginUpdateResource(string fileName, bool deleteExisting);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool UpdateResource(IntPtr hUpdate, IntPtr type, IntPtr name, ushort language, byte[] data, uint size);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool EndUpdateResource(IntPtr hUpdate, bool discard);
}
"@

# Parse the .ico into individual images + a RT_GROUP_ICON blob.
$ico = [System.IO.File]::ReadAllBytes($IcoPath)
if ($ico.Length -lt 6 -or $ico[0] -ne 0 -or $ico[1] -ne 0) { throw "Not a valid ICO file: $IcoPath" }
$count = [BitConverter]::ToUInt16($ico, 4)
if ($count -lt 1) { throw "ICO has no images: $IcoPath" }

$entries = @()
for ($i = 0; $i -lt $count; $i++) {
    $off = 6 + $i * 16
    $entries += [pscustomobject]@{
        Width      = $ico[$off]
        Height     = $ico[$off + 1]
        ColorCount = $ico[$off + 2]
        Reserved   = $ico[$off + 3]
        Planes     = [BitConverter]::ToUInt16($ico, $off + 4)
        BitCount   = [BitConverter]::ToUInt16($ico, $off + 6)
        BytesInRes = [BitConverter]::ToUInt32($ico, $off + 8)
        ImageOff   = [BitConverter]::ToUInt32($ico, $off + 12)
    }
}

# GRPICONDIR: same as ICONDIR but count is number of images.
$group = New-Object System.Collections.Generic.List[byte]
$group.AddRange([BitConverter]::GetBytes([uint16]0))  # reserved
$group.AddRange([BitConverter]::GetBytes([uint16]1))  # type = icon
$group.AddRange([BitConverter]::GetBytes([uint16]$count))
# GRPICONDIRENTRY: width,height,color,reserved,planes,bitcount,bytes,id
for ($i = 0; $i -lt $count; $i++) {
    $e = $entries[$i]
    $group.Add([byte]$e.Width)
    $group.Add([byte]$e.Height)
    $group.Add([byte]$e.ColorCount)
    $group.Add([byte]0)
    $group.AddRange([BitConverter]::GetBytes([uint16]$e.Planes))
    $group.AddRange([BitConverter]::GetBytes([uint16]$e.BitCount))
    $group.AddRange([BitConverter]::GetBytes([uint32]$e.BytesInRes))
    $group.AddRange([BitConverter]::GetBytes([uint16]($i + 1)))  # nID
}

$RT_ICON = [IntPtr]3
$RT_GROUP_ICON = [IntPtr]14
$LANG = [uint16]0x0409  # en-US

$h = [IconUpdater]::BeginUpdateResource($ExePath, $false)
if ($h -eq [IntPtr]::Zero) { throw "BeginUpdateResource failed: $([ComponentModel.Win32Exception]::new([Runtime.InteropServices.Marshal]::GetLastWin32Error()).Message)" }

$ok = $true
for ($i = 0; $i -lt $count; $i++) {
    $e = $entries[$i]
    $img = New-Object byte[] $e.BytesInRes
    [Array]::Copy($ico, [int]$e.ImageOff, $img, 0, [int]$e.BytesInRes)
    $id = [IntPtr]($i + 1)
    if (-not [IconUpdater]::UpdateResource($h, $RT_ICON, $id, $LANG, $img, [uint32]$img.Length)) {
        $ok = $false
        break
    }
}
if ($ok) {
    $groupBytes = $group.ToArray()
    $groupId = [IntPtr]1
    $ok = [IconUpdater]::UpdateResource($h, $RT_GROUP_ICON, $groupId, $LANG, $groupBytes, [uint32]$groupBytes.Length)
}

if (-not [IconUpdater]::EndUpdateResource($h, -not $ok)) {
    $ok = $false
}
if (-not $ok) { throw "UpdateResource failed: $([ComponentModel.Win32Exception]::new([Runtime.InteropServices.Marshal]::GetLastWin32Error()).Message)" }

Write-Host "Embedded icon into: $ExePath ($count images)"
