#!/bin/bash
# Build ViKey Rust core for macOS (Apple Silicon)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
CORE_DIR="$PROJECT_ROOT/core"

echo "Building ViKey core for macOS (arm64)..."

# Check Rust target
if ! rustup target list --installed | grep -q "aarch64-apple-darwin"; then
    echo "Installing Rust target for Apple Silicon..."
    rustup target add aarch64-apple-darwin
fi

# Build
cd "$CORE_DIR"
cargo build --release --target aarch64-apple-darwin

# Output location
OUTPUT="$CORE_DIR/target/aarch64-apple-darwin/release/libvikey_core.a"

if [ -f "$OUTPUT" ]; then
    echo ""
    echo "Build successful!"
    echo "Static library: $OUTPUT"
    echo ""

    # Copy to app-macos for Xcode
    MACOS_LIB="$PROJECT_ROOT/app-macos/ViKey/lib"
    mkdir -p "$MACOS_LIB"
    cp "$OUTPUT" "$MACOS_LIB/"
    echo "Copied to: $MACOS_LIB/libvikey_core.a"
else
    echo "Build failed!"
    exit 1
fi
