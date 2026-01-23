# ViKey - Build Rust core library for Windows
# Produces: core/target/release/vikey_core.dll

param(
    [switch]$Release = $true
)

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$rootDir = Split-Path -Parent $scriptDir
$coreDir = Join-Path $rootDir "core"

Write-Host "Building Rust core library..." -ForegroundColor Cyan

Push-Location $coreDir
try {
    if ($Release) {
        cargo build --release
    } else {
        cargo build
    }

    if ($LASTEXITCODE -ne 0) {
        throw "Cargo build failed with exit code $LASTEXITCODE"
    }

    $dllPath = if ($Release) {
        Join-Path $coreDir "target\release\vikey_core.dll"
    } else {
        Join-Path $coreDir "target\debug\vikey_core.dll"
    }

    if (Test-Path $dllPath) {
        $size = (Get-Item $dllPath).Length / 1KB
        Write-Host "Build successful: $dllPath ($([math]::Round($size, 2)) KB)" -ForegroundColor Green
    } else {
        throw "DLL not found at: $dllPath"
    }
}
finally {
    Pop-Location
}
