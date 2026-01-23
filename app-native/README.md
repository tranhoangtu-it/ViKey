# ViKey - Native Win32

Ứng dụng C++ Win32 thuần cho ViKey - Bộ gõ tiếng Việt. Dung lượng tối thiểu (~1MB).

## Kiến trúc

```
app-native/
├── src/
│   ├── main.cpp              # Entry point, message loop, dialogs
│   ├── keyboard_hook.cpp/.h  # Low-level keyboard hook (WH_KEYBOARD_LL)
│   ├── text_sender.cpp/.h    # SendInput với KEYEVENTF_UNICODE
│   ├── rust_bridge.cpp/.h    # FFI tới core.dll
│   ├── ime_processor.cpp/.h  # Điều phối chính
│   ├── tray_icon.cpp/.h      # System tray (Shell_NotifyIcon)
│   ├── settings.cpp/.h       # Lưu cài đặt vào Registry
│   ├── hotkey.cpp/.h         # Global hotkey tuỳ chỉnh
│   ├── shortcut_manager.cpp/.h # Gõ tắt (vn -> Việt Nam)
│   ├── keycodes.cpp/.h       # Ánh xạ VK sang macOS keycode
│   ├── resource.h            # Resource IDs
│   └── resource.rc           # Menu, dialog, version info
├── ViKey.vcxproj             # Visual Studio project
└── README.md
```

## Yêu cầu Build

- Visual Studio 2022 (hoặc 2019) với:
  - Desktop development with C++
  - Windows 10 SDK
- HOẶC Visual Studio Build Tools 2022

## Build

```powershell
# Build Release
.\scripts\build-native.ps1

# Build Debug
.\scripts\build-native.ps1 -Configuration Debug

# Clean build
.\scripts\build-native.ps1 -Clean
```

Hoặc dùng Visual Studio:
1. Mở `app-native\ViKey.vcxproj`
2. Chọn Release | x64
3. Build → Build Solution (Ctrl+Shift+B)

## Output

```
app-native\bin\Release\
├── ViKey.exe     # ~300KB
└── core.dll      # ~700KB (từ Rust core)
```

Tổng dung lượng: ~1MB

## Đóng gói

```powershell
.\scripts\package-native.ps1 -Version "1.2.0"
```

Tạo: `dist\ViKey-v1.2.0-win64.zip`

## Tính năng

- **Low-level keyboard hook** - Bắt phím toàn hệ thống
- **Unicode input** - Hỗ trợ đầy đủ ký tự tiếng Việt qua SendInput
- **System tray** - Icon động V/E, context menu
- **Global hotkey** - Phím tắt tuỳ chỉnh
- **Gõ tắt** - Mở rộng viết tắt (vn → Việt Nam)
- **Lưu cài đặt** - Registry-based
- **Single instance** - Mutex-based detection

## Thông số kỹ thuật

| Thông số | Giá trị |
|----------|---------|
| Dung lượng | ~1MB |
| Runtime | Không cần |
| Thời gian khởi động | ~50ms |
| RAM | ~5MB |
| UI Framework | Raw Win32 |
| System tray | Shell_NotifyIcon |
| Cài đặt UI | Resource dialogs |

## Ghi chú kỹ thuật

1. **Keyboard Hook**: Sử dụng `WH_KEYBOARD_LL` với user32.dll module handle.

2. **SendInput**: Căn chỉnh struct 64-bit quan trọng. INPUT struct phải 40 bytes với đúng field offsets.

3. **Injected Key Marker**: `0x564E494D` ("VNIM") trong dwExtraInfo để nhận diện phím được inject.

4. **GDI+ Icons**: Tạo icon V/E động dùng GDI+ cho text rendering anti-aliased.

5. **Registry**: Cài đặt lưu tại `HKCU\SOFTWARE\ViKey`, auto-start trong Run key.

## Tích hợp Rust Core

Native app load `core.dll` qua LoadLibrary và GetProcAddress:

```cpp
// Các hàm cần thiết
ime_init()           - Khởi tạo engine
ime_key()            - Xử lý keystroke
ime_key_ext()        - Xử lý với tham số shift
ime_free()           - Giải phóng bộ nhớ result
ime_clear()          - Xoá buffer
ime_enabled()        - Bật/tắt
ime_method()         - Đặt Telex/VNI
// ... và các hàm cài đặt khác
```

Result struct (1028 bytes):
```cpp
struct NativeResult {
    uint32_t chars[256];  // UTF-32 code points
    uint8_t action;       // 0=None, 1=Send, 2=Restore
    uint8_t backspace;    // Số ký tự cần xoá
    uint8_t count;        // Số ký tự cần gửi
    uint8_t flags;        // FLAG_KEY_CONSUMED=0x01
};
```
