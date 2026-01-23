# ViKey - Native Build Script
# build-native.ps1
# Builds the C++ Win32 application

param(
    [string]$Configuration = "Release",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$projectDir = "$PSScriptRoot\..\app-native"
$outputDir = "$projectDir\bin\$Configuration"

Write-Host "=== ViKey Native Build ===" -ForegroundColor Cyan
Write-Host "Configuration: $Configuration"
Write-Host ""

# Find MSBuild
$msbuildPaths = @(
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
)

$msbuild = $null
foreach ($path in $msbuildPaths) {
    if (Test-Path $path) {
        $msbuild = $path
        break
    }
}

if (-not $msbuild) {
    # Try to find via vswhere
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswhere) {
        $installPath = & $vswhere -latest -property installationPath
        if ($installPath) {
            $msbuild = Join-Path $installPath "MSBuild\Current\Bin\MSBuild.exe"
        }
    }
}

if (-not $msbuild -or -not (Test-Path $msbuild)) {
    Write-Host "ERROR: MSBuild not found. Please install Visual Studio 2019/2022 or Build Tools." -ForegroundColor Red
    exit 1
}

Write-Host "Using MSBuild: $msbuild"
Write-Host ""

# Clean if requested
if ($Clean) {
    Write-Host "Cleaning..." -ForegroundColor Yellow
    if (Test-Path "$projectDir\bin") {
        Remove-Item -Recurse -Force "$projectDir\bin"
    }
    if (Test-Path "$projectDir\obj") {
        Remove-Item -Recurse -Force "$projectDir\obj"
    }
}

# Build
Write-Host "Building $Configuration..." -ForegroundColor Yellow
$projectFile = "$projectDir\ViKey.vcxproj"

& $msbuild $projectFile /p:Configuration=$Configuration /p:Platform=x64 /m /nologo /v:minimal

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "BUILD FAILED" -ForegroundColor Red
    exit $LASTEXITCODE
}

# Copy Rust core DLL (rename vikey_core.dll to core.dll)
$coreDllSrc = "$PSScriptRoot\..\core\target\release\vikey_core.dll"
$coreDllDst = "$outputDir\core.dll"
if (Test-Path $coreDllSrc) {
    Write-Host "Copying core.dll..." -ForegroundColor Yellow
    Copy-Item $coreDllSrc $coreDllDst -Force
} else {
    Write-Host "WARNING: vikey_core.dll not found at $coreDllSrc" -ForegroundColor Yellow
    Write-Host "Run build-core.ps1 first to build the Rust core" -ForegroundColor Yellow
}

# Show output
Write-Host ""
Write-Host "BUILD SUCCEEDED" -ForegroundColor Green
Write-Host ""
Write-Host "Output directory: $outputDir"
Write-Host ""

# List output files
Get-ChildItem $outputDir | ForEach-Object {
    $size = "{0:N0} KB" -f ($_.Length / 1KB)
    Write-Host "  $($_.Name) ($size)"
}

Write-Host ""
Write-Host "To run: $outputDir\ViKey.exe"
