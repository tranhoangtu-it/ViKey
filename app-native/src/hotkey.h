// ViKey - Global Hotkey Manager
// hotkey.h
// Registers configurable global toggle hotkey

#pragma once

#include <windows.h>
#include <functional>
#include "settings.h"

class HotkeyManager {
public:
    static HotkeyManager& Instance();

    // Register toggle hotkey from settings
    bool Register(HWND hWnd);

    // Register with specific config
    bool Register(HWND hWnd, const HotkeyConfig& config);

    // Unregister the hotkey
    void Unregister(HWND hWnd);

    // Re-register with new settings (call after settings change)
    bool UpdateHotkey(HWND hWnd);

    // Check if hotkey is registered
    bool IsRegistered() const { return m_registered; }

    // Set callback for hotkey press
    void SetCallback(std::function<void()> callback) { m_callback = callback; }

    // Process WM_HOTKEY message, returns true if handled
    bool ProcessHotkey(WPARAM wParam);

    // Get hotkey ID
    static constexpr int HOTKEY_ID = 9000;

private:
    HotkeyManager();
    ~HotkeyManager() = default;
    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;

    HWND m_hWnd;
    bool m_registered;
    std::function<void()> m_callback;
};
