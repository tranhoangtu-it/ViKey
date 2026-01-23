// ViKey - Key Code Mapping
// keycodes.h
// Maps Windows virtual key codes to macOS keycodes used by Rust engine

#pragma once

#include <windows.h>
#include <cstdint>

// Windows VK codes - Control keys
constexpr int VK_BACK_KEY = 0x08;
constexpr int VK_TAB_KEY = 0x09;
constexpr int VK_RETURN_KEY = 0x0D;
constexpr int VK_SHIFT_KEY = 0x10;
constexpr int VK_CONTROL_KEY = 0x11;
constexpr int VK_MENU_KEY = 0x12;    // Alt key
constexpr int VK_CAPITAL_KEY = 0x14; // Caps Lock
constexpr int VK_ESCAPE_KEY = 0x1B;
constexpr int VK_SPACE_KEY = 0x20;
constexpr int VK_LEFT_KEY = 0x25;
constexpr int VK_UP_KEY = 0x26;
constexpr int VK_RIGHT_KEY = 0x27;
constexpr int VK_DOWN_KEY = 0x28;

// Windows VK codes - Alphanumeric
constexpr int VK_0_KEY = 0x30;
constexpr int VK_9_KEY = 0x39;
constexpr int VK_A_KEY = 0x41;
constexpr int VK_Z_KEY = 0x5A;

// Windows VK codes - OEM keys
constexpr int VK_OEM_1_KEY = 0xBA;      // ;:
constexpr int VK_OEM_PLUS_KEY = 0xBB;   // =+
constexpr int VK_OEM_COMMA_KEY = 0xBC;  // ,<
constexpr int VK_OEM_MINUS_KEY = 0xBD;  // -_
constexpr int VK_OEM_PERIOD_KEY = 0xBE; // .>
constexpr int VK_OEM_2_KEY = 0xBF;      // /?
constexpr int VK_OEM_3_KEY = 0xC0;      // `~
constexpr int VK_OEM_4_KEY = 0xDB;      // [{
constexpr int VK_OEM_5_KEY = 0xDC;      // \|
constexpr int VK_OEM_6_KEY = 0xDD;      // ]}
constexpr int VK_OEM_7_KEY = 0xDE;      // '"

namespace KeyCodes {
    // Convert Windows VK code to macOS keycode for Rust engine
    uint16_t ToMacKeycode(int vkCode);

    // Check if VK code is a letter (A-Z)
    bool IsLetter(int vkCode);

    // Check if VK code is a number (0-9)
    bool IsNumber(int vkCode);

    // Check if key is relevant for IME processing
    bool IsRelevantKey(int vkCode);

    // Check if key should clear the typing buffer (word boundary)
    bool IsBufferClearKey(int vkCode);

    // Convert VK code to character (for shortcut tracking)
    // Returns 0 if not a valid character key
    char ToChar(int vkCode, bool shift, bool capsLock);
}
