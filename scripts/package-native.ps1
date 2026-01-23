# ViKey - Native Package Script
# package-native.ps1
# Creates a portable zip distribution

param(
    [string]$Version = "1.2.0"
)

$ErrorActionPreference = "Stop"
$scriptDir = $PSScriptRoot
$projectRoot = "$scriptDir\.."
$nativeOutput = "$projectRoot\app-native\bin\Release"
$distDir = "$projectRoot\dist"
$packageName = "ViKey-v$Version-win64"

Write-Host "=== ViKey Native Packaging ===" -ForegroundColor Cyan
Write-Host "Version: $Version"
Write-Host ""

# Check if build exists
if (-not (Test-Path "$nativeOutput\ViKey.exe")) {
    Write-Host "ERROR: ViKey.exe not found. Run build-native.ps1 first." -ForegroundColor Red
    exit 1
}

# Check for core DLL
if (-not (Test-Path "$nativeOutput\core.dll")) {
    Write-Host "ERROR: core.dll not found. Run build-core.ps1 first." -ForegroundColor Red
    exit 1
}

# Create dist directory
if (-not (Test-Path $distDir)) {
    New-Item -ItemType Directory -Path $distDir | Out-Null
}

# Create package directory
$packageDir = "$distDir\$packageName"
if (Test-Path $packageDir) {
    Remove-Item -Recurse -Force $packageDir
}
New-Item -ItemType Directory -Path $packageDir | Out-Null

Write-Host "Copying files..." -ForegroundColor Yellow

# Copy required files
Copy-Item "$nativeOutput\ViKey.exe" $packageDir
Copy-Item "$nativeOutput\core.dll" $packageDir

# Create README
$readme = @"
ViKey v$Version
===============

Nhe, nhanh, chuan Viet - Vietnamese keyboard for Windows.

Usage:
1. Run ViKey.exe
2. Use Ctrl+Space to toggle Vietnamese/English mode
3. Type using Telex or VNI input method

System Tray:
- Right-click the tray icon for menu
- Double-click to toggle mode

Requirements:
- Windows 10 or later (x64)
- No .NET runtime required

Files:
- ViKey.exe - Main application (~300KB)
- core.dll  - Vietnamese processing engine (~700KB)
"@
$readme | Out-File "$packageDir\README.txt" -Encoding UTF8

# Create zip
$zipPath = "$distDir\$packageName.zip"
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}

Write-Host "Creating zip archive..." -ForegroundColor Yellow
Compress-Archive -Path "$packageDir\*" -DestinationPath $zipPath

# Show results
Write-Host ""
Write-Host "PACKAGING COMPLETE" -ForegroundColor Green
Write-Host ""
Write-Host "Package contents:"

Get-ChildItem $packageDir | ForEach-Object {
    $size = "{0:N0} KB" -f ($_.Length / 1KB)
    Write-Host "  $($_.Name) ($size)"
}

$zipSize = (Get-Item $zipPath).Length / 1KB
Write-Host ""
Write-Host "Zip archive: $zipPath ({0:N0} KB)" -f $zipSize

# Cleanup package directory (keep only zip)
Remove-Item -Recurse -Force $packageDir

Write-Host ""
Write-Host "Done!"
