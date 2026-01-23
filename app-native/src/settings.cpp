// ViKey - Settings Manager Implementation
// settings.cpp
// Project: ViKey | Author: Trần Công Sinh | https://github.com/kmis8x/ViKey

#include "settings.h"
#include <shlwapi.h>
#include <sstream>

#pragma comment(lib, "shlwapi.lib")

Settings& Settings::Instance() {
    static Settings instance;
    return instance;
}

Settings::Settings()
    : enabled(true)
    , method(InputMethod::Telex)
    , modernTone(true)
    , englishAutoRestore(true)
    , autoCapitalize(false)
    , escRestore(true)
    , freeTone(false)
    , skipWShortcut(false)
    , bracketShortcut(false)
    , slowMode(false)
    , autoStart(false)
    , silentStartup(false) {
}

void Settings::Load() {
    enabled = GetBool(L"Enabled", true);
    method = static_cast<InputMethod>(GetInt(L"Method", 0));
    modernTone = GetBool(L"ModernTone", true);
    englishAutoRestore = GetBool(L"EnglishAutoRestore", true);
    autoCapitalize = GetBool(L"AutoCapitalize", false);
    escRestore = GetBool(L"EscRestore", true);
    freeTone = GetBool(L"FreeTone", false);
    skipWShortcut = GetBool(L"SkipWTextShortcut", false);
    bracketShortcut = GetBool(L"BracketTextShortcut", false);
    slowMode = GetBool(L"SlowMode", false);
    autoStart = GetAutoStart();
    silentStartup = GetBool(L"SilentStartup", false);
    LoadShortcuts();

    // Load toggle hotkey config
    toggleHotkey.ctrl = GetBool(L"HotkeyCtrl", true);
    toggleHotkey.shift = GetBool(L"HotkeyShift", false);
    toggleHotkey.alt = GetBool(L"HotkeyAlt", false);
    toggleHotkey.win = GetBool(L"HotkeyWin", false);
    toggleHotkey.vkCode = static_cast<UINT>(GetInt(L"HotkeyKey", VK_SPACE));
}

void Settings::Save() {
    SetBool(L"Enabled", enabled);
    SetInt(L"Method", static_cast<int>(method));
    SetBool(L"ModernTone", modernTone);
    SetBool(L"EnglishAutoRestore", englishAutoRestore);
    SetBool(L"AutoCapitalize", autoCapitalize);
    SetBool(L"EscRestore", escRestore);
    SetBool(L"FreeTone", freeTone);
    SetBool(L"SkipWTextShortcut", skipWShortcut);
    SetBool(L"BracketTextShortcut", bracketShortcut);
    SetBool(L"SlowMode", slowMode);
    SetAutoStart(autoStart);
    SetBool(L"SilentStartup", silentStartup);
    SaveShortcuts();

    // Save toggle hotkey config
    SetBool(L"HotkeyCtrl", toggleHotkey.ctrl);
    SetBool(L"HotkeyShift", toggleHotkey.shift);
    SetBool(L"HotkeyAlt", toggleHotkey.alt);
    SetBool(L"HotkeyWin", toggleHotkey.win);
    SetInt(L"HotkeyKey", static_cast<int>(toggleHotkey.vkCode));
}

std::vector<TextShortcut> Settings::DefaultShortcuts() {
    return {
        {L"vn", L"Vi\u1EC7t Nam"},
        {L"hn", L"H\u00E0 N\u1ED9i"},
        {L"tphcm", L"Th\u00E0nh ph\u1ED1 H\u1ED3 Ch\u00ED Minh"},
        {L"sg", L"S\u00E0i G\u00F2n"},
        {L"ko", L"kh\u00F4ng"},
        {L"dc", L"\u0111\u01B0\u1EE3c"},
        {L"nc", L"n\u01B0\u1EDBc"},
        {L"bn", L"b\u1EA1n"},
        {L"mk", L"m\u00ECnh"},
        {L"ns", L"n\u00F3i"},
        {L"vs", L"v\u1EDBi"},
        {L"ntn", L"nh\u01B0 th\u1EBF n\u00E0o"},
        {L"j", L"g\u00EC"},
        {L"cx", L"c\u0169ng"},
        {L"ng", L"ng\u01B0\u1EDDi"},
        {L"ck", L"ch\u1ED3ng"},
        {L"vk", L"v\u1EE3"},
        {L"bt", L"b\u00ECnh th\u01B0\u1EDDng"},
        {L"nt", L"nh\u1EAFn tin"},
        {L"ctv", L"c\u1ED9ng t\u00E1c vi\u00EAn"},
    };
}

bool Settings::GetBool(const wchar_t* name, bool defaultValue) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REGISTRY_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return defaultValue;
    }

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = REG_DWORD;
    bool result = defaultValue;

    if (RegQueryValueExW(hKey, name, nullptr, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
        result = value != 0;
    }

    RegCloseKey(hKey);
    return result;
}

void Settings::SetBool(const wchar_t* name, bool value) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REGISTRY_PATH, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        return;
    }

    DWORD dwordValue = value ? 1 : 0;
    RegSetValueExW(hKey, name, 0, REG_DWORD, (LPBYTE)&dwordValue, sizeof(dwordValue));
    RegCloseKey(hKey);
}

int Settings::GetInt(const wchar_t* name, int defaultValue) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REGISTRY_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return defaultValue;
    }

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = REG_DWORD;
    int result = defaultValue;

    if (RegQueryValueExW(hKey, name, nullptr, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
        result = static_cast<int>(value);
    }

    RegCloseKey(hKey);
    return result;
}

void Settings::SetInt(const wchar_t* name, int value) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REGISTRY_PATH, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        return;
    }

    DWORD dwordValue = static_cast<DWORD>(value);
    RegSetValueExW(hKey, name, 0, REG_DWORD, (LPBYTE)&dwordValue, sizeof(dwordValue));
    RegCloseKey(hKey);
}

std::wstring Settings::GetString(const wchar_t* name, const wchar_t* defaultValue) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REGISTRY_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return defaultValue;
    }

    wchar_t buffer[4096] = {0};
    DWORD size = sizeof(buffer);
    DWORD type = REG_SZ;
    std::wstring result = defaultValue;

    if (RegQueryValueExW(hKey, name, nullptr, &type, (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
        result = buffer;
    }

    RegCloseKey(hKey);
    return result;
}

void Settings::SetString(const wchar_t* name, const std::wstring& value) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REGISTRY_PATH, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        return;
    }

    RegSetValueExW(hKey, name, 0, REG_SZ,
                   (LPBYTE)value.c_str(), static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);
}

bool Settings::GetAutoStart() const {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, STARTUP_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    wchar_t buffer[MAX_PATH] = {0};
    DWORD size = sizeof(buffer);
    bool exists = RegQueryValueExW(hKey, APP_NAME, nullptr, nullptr, (LPBYTE)buffer, &size) == ERROR_SUCCESS;

    RegCloseKey(hKey);
    return exists;
}

void Settings::SetAutoStart(bool enabled) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, STARTUP_PATH, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        return;
    }

    if (enabled) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring value = L"\"" + std::wstring(exePath) + L"\"";
        RegSetValueExW(hKey, APP_NAME, 0, REG_SZ,
                       (LPBYTE)value.c_str(), static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
    } else {
        RegDeleteValueW(hKey, APP_NAME);
    }

    RegCloseKey(hKey);
}

void Settings::LoadShortcuts() {
    std::wstring json = GetString(L"TextShortcuts", L"");
    if (json.empty()) {
        shortcuts = DefaultShortcuts();
        return;
    }

    // Simple JSON-like parsing: key1|value1;key2|value2;...
    shortcuts.clear();
    std::wstring::size_type pos = 0;
    while (pos < json.length()) {
        auto semicolon = json.find(L';', pos);
        if (semicolon == std::wstring::npos) semicolon = json.length();

        std::wstring pair = json.substr(pos, semicolon - pos);
        auto pipe = pair.find(L'|');
        if (pipe != std::wstring::npos) {
            TextShortcut s;
            s.key = pair.substr(0, pipe);
            s.value = pair.substr(pipe + 1);
            if (!s.key.empty() && !s.value.empty()) {
                shortcuts.push_back(s);
            }
        }

        pos = semicolon + 1;
    }

    if (shortcuts.empty()) {
        shortcuts = DefaultShortcuts();
    }
}

void Settings::SaveShortcuts() {
    // Simple serialization: key1|value1;key2|value2;...
    std::wstring json;
    for (const auto& s : shortcuts) {
        if (!json.empty()) json += L';';
        json += s.key + L'|' + s.value;
    }
    SetString(L"TextShortcuts", json);
}
