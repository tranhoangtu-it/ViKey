// ViKey - Text Sender Implementation
// text_sender.cpp
// Project: ViKey | Author: Trần Công Sinh | https://github.com/kmis8x/ViKey

#include "text_sender.h"
#include "keyboard_hook.h"
#include "keycodes.h"
#include <vector>

// Win32 Constants
constexpr DWORD INPUT_KEYBOARD_TYPE = 1;
constexpr DWORD KEYEVENTF_KEYUP_FLAG = 0x0002;
constexpr DWORD KEYEVENTF_UNICODE_FLAG = 0x0004;

// INPUT struct for SendInput (explicit layout for 64-bit compatibility)
#pragma pack(push, 8)
struct INPUT_DATA {
    DWORD type;
    union {
        struct {
            WORD wVk;
            WORD wScan;
            DWORD dwFlags;
            DWORD time;
            ULONG_PTR dwExtraInfo;
            DWORD padding[2]; // Padding for 64-bit alignment
        } ki;
    };
};
#pragma pack(pop)

TextSender& TextSender::Instance() {
    static TextSender instance;
    return instance;
}

TextSender::TextSender() : m_slowMode(false) {}

void TextSender::SendText(const std::wstring& text, int backspaces) {
    if (text.empty() && backspaces == 0) return;

    if (m_slowMode) {
        SendTextSlow(text, backspaces);
    } else {
        SendTextFast(text, backspaces);
    }
}

void TextSender::SendTextFast(const std::wstring& text, int backspaces) {
    // Use keybd_event for backspaces (better compatibility with remote desktop)
    for (int i = 0; i < backspaces; i++) {
        keybd_event(VK_BACK, 0x0E, 0, INJECTED_KEY_MARKER);
        Sleep(5);
        keybd_event(VK_BACK, 0x0E, KEYEVENTF_KEYUP_FLAG, INJECTED_KEY_MARKER);
        Sleep(5);
    }

    // Delay between backspaces and text
    if (backspaces > 0) {
        Sleep(10);
    }

    // Send Unicode characters using SendInput (keybd_event doesn't support Unicode)
    INPUT input[2] = {};
    input[0].type = INPUT_KEYBOARD;
    input[1].type = INPUT_KEYBOARD;

    for (wchar_t c : text) {
        if (c >= 0xD800 && c <= 0xDBFF) continue;

        input[0].ki.wVk = 0;
        input[0].ki.wScan = c;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE_FLAG;
        input[0].ki.dwExtraInfo = INJECTED_KEY_MARKER;

        input[1].ki.wVk = 0;
        input[1].ki.wScan = c;
        input[1].ki.dwFlags = KEYEVENTF_UNICODE_FLAG | KEYEVENTF_KEYUP_FLAG;
        input[1].ki.dwExtraInfo = INJECTED_KEY_MARKER;

        SendInput(2, input, sizeof(INPUT));
        Sleep(5);
    }
}

void TextSender::SendTextSlow(const std::wstring& text, int backspaces) {
    // Use keybd_event for backspaces with longer delays
    for (int i = 0; i < backspaces; i++) {
        keybd_event(VK_BACK, 0x0E, 0, INJECTED_KEY_MARKER);
        Sleep(15);
        keybd_event(VK_BACK, 0x0E, KEYEVENTF_KEYUP_FLAG, INJECTED_KEY_MARKER);
        Sleep(15);
    }

    // Longer delay between backspaces and text
    if (backspaces > 0) {
        Sleep(30);
    }

    // Send Unicode characters using SendInput
    INPUT input[2] = {};
    input[0].type = INPUT_KEYBOARD;
    input[1].type = INPUT_KEYBOARD;

    for (wchar_t c : text) {
        if (c >= 0xD800 && c <= 0xDBFF) continue;

        input[0].ki.wVk = 0;
        input[0].ki.wScan = c;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE_FLAG;
        input[0].ki.dwExtraInfo = INJECTED_KEY_MARKER;

        input[1].ki.wVk = 0;
        input[1].ki.wScan = c;
        input[1].ki.dwFlags = KEYEVENTF_UNICODE_FLAG | KEYEVENTF_KEYUP_FLAG;
        input[1].ki.dwExtraInfo = INJECTED_KEY_MARKER;

        SendInput(2, input, sizeof(INPUT));
        Sleep(15);
    }
}
