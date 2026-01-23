// ViKey - Text Sender
// text_sender.h
// Sends text using Win32 SendInput API with KEYEVENTF_UNICODE

#pragma once

#include <windows.h>
#include <string>

class TextSender {
public:
    static TextSender& Instance();

    // Slow mode: send events individually with small delays (for terminals)
    void SetSlowMode(bool slow) { m_slowMode = slow; }
    bool IsSlowMode() const { return m_slowMode; }

    // Send text replacement: delete characters then insert new text
    void SendText(const std::wstring& text, int backspaces);

private:
    TextSender();
    ~TextSender() = default;
    TextSender(const TextSender&) = delete;
    TextSender& operator=(const TextSender&) = delete;

    // Fast mode: batch all events (default)
    void SendTextFast(const std::wstring& text, int backspaces);

    // Slow mode: send events one by one with delays (for problematic apps)
    void SendTextSlow(const std::wstring& text, int backspaces);

    bool m_slowMode;
};
