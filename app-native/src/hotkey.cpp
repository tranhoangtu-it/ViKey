// ViKey - Global Hotkey Manager Implementation
// hotkey.cpp

#include "hotkey.h"

HotkeyManager& HotkeyManager::Instance() {
    static HotkeyManager instance;
    return instance;
}

HotkeyManager::HotkeyManager()
    : m_hWnd(nullptr)
    , m_registered(false)
    , m_callback(nullptr) {
}

bool HotkeyManager::Register(HWND hWnd) {
    return Register(hWnd, Settings::Instance().toggleHotkey);
}

bool HotkeyManager::Register(HWND hWnd, const HotkeyConfig& config) {
    if (m_registered) return true;

    m_hWnd = hWnd;
    UINT modifiers = config.GetModifiers();
    m_registered = RegisterHotKey(hWnd, HOTKEY_ID, modifiers, config.vkCode);
    return m_registered;
}

void HotkeyManager::Unregister(HWND hWnd) {
    if (m_registered) {
        UnregisterHotKey(hWnd, HOTKEY_ID);
        m_registered = false;
    }
}

bool HotkeyManager::UpdateHotkey(HWND hWnd) {
    Unregister(hWnd);
    return Register(hWnd);
}

bool HotkeyManager::ProcessHotkey(WPARAM wParam) {
    if (static_cast<int>(wParam) == HOTKEY_ID) {
        if (m_callback) {
            m_callback();
        }
        return true;
    }
    return false;
}
