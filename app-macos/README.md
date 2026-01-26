# ViKey macOS

Bộ gõ tiếng Việt cho macOS sử dụng Input Method Kit.

## Yêu cầu

- macOS 12.0+ (Monterey)
- Xcode 14.0+ (hoặc Command Line Tools)
- Rust 1.70+
- Apple Silicon (M1/M2/M3)

## Build

### 1. Cài đặt Rust target cho macOS ARM64

```bash
rustup target add aarch64-apple-darwin
```

### 2. Build

```bash
cd app-macos
chmod +x build.sh
./build.sh
```

### 3. Cài đặt

```bash
cp -r build/ViKey.app ~/Library/Input\ Methods/
```

### 4. Kích hoạt

1. Mở **System Preferences** → **Keyboard** → **Input Sources**
2. Click **+** → Tìm **ViKey**
3. Thêm **ViKey Vietnamese**
4. Chọn ViKey từ menu input trên thanh menu

## Cấu trúc

```
app-macos/
├── ViKey/
│   ├── Sources/
│   │   ├── main.swift              # Entry point
│   │   ├── ViKeyInputController.swift  # IMKit controller
│   │   ├── RustBridge.swift        # FFI wrapper
│   │   ├── Settings.swift          # UserDefaults settings
│   │   └── ViKey-Bridging-Header.h # C headers for Rust FFI
│   ├── Resources/
│   │   └── Info.plist              # Bundle configuration
│   └── lib/
│       └── libvikey_core.a         # Rust static library (after build)
├── build.sh                        # Build script
└── README.md
```

## Sử dụng

- **Ctrl+Space**: Bật/tắt gõ tiếng Việt
- **Telex**: aa→â, ow→ơ, s→sắc, dd→đ
- **VNI**: a6→â, o7→ơ, 1→sắc, d9→đ

## Cài đặt mặc định

| Tùy chọn | Mặc định |
|----------|----------|
| Kiểu gõ | Telex |
| Bỏ dấu kiểu mới | Bật (hoà) |
| ESC khôi phục | Bật |
| Tự động khôi phục tiếng Anh | Bật |

## Troubleshooting

### Input Method không xuất hiện

1. Logout và login lại
2. Hoặc chạy: `killall -HUP SystemUIServer`

### Không gõ được

1. Kiểm tra đã chọn ViKey trong Input Sources
2. Thử **Ctrl+Space** để bật/tắt

### Build lỗi

1. Đảm bảo đã cài Rust: `rustup target list --installed`
2. Kiểm tra Xcode Command Line Tools: `xcode-select --install`
