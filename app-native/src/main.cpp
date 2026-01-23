// ViKey - Win32 Native Application
// main.cpp
// Entry point, message loop, window management
//
// Project: ViKey | Author: Trần Công Sinh | https://github.com/kmis8x/ViKey

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <commctrl.h>

#include "resource.h"
#include "ime_processor.h"
#include "tray_icon.h"
#include "hotkey.h"
#include "settings.h"
#include "keyboard_hook.h"
#include <cstdio>
#include <ctime>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Application name and class
constexpr const wchar_t* APP_NAME = L"ViKey";
constexpr const wchar_t* WINDOW_CLASS = L"ViKey_Hidden";
constexpr const wchar_t* MUTEX_NAME = L"Global\\ViKey_SingleInstance";

// Custom messages
constexpr UINT WM_TRAYICON_MSG = WM_USER + 1;

// Global variables
HINSTANCE g_hInstance = nullptr;
HWND g_hWnd = nullptr;
ULONG_PTR g_gdiplusToken = 0;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool InitInstance(HINSTANCE hInstance);
void CleanupInstance();
INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateUI();
void ShowSettingsDialog();
void ShowAboutDialog();

// Entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Check for single instance
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, MUTEX_NAME);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // Another instance is already running
        if (hMutex) CloseHandle(hMutex);
        return 0;
    }

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    // Initialize common controls
    INITCOMMONCONTROLSEX icc = {};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    // Initialize application
    if (!InitInstance(hInstance)) {
        CleanupInstance();
        if (hMutex) CloseHandle(hMutex);
        return 1;
    }

    // Debug: log that we're entering message loop
    {
        FILE* f = fopen("D:\\CK\\ViKey\\app-native\\msgloop_start.log", "w");
        if (f) {
            fprintf(f, "Entering message loop, g_hWnd=%p, thread=%lu\n",
                    (void*)g_hWnd, GetCurrentThreadId());
            fclose(f);
        }
    }

    // Set up a timer to periodically verify hook status
    SetTimer(g_hWnd, 999, 3000, nullptr);  // Every 3 seconds

    // Message loop using GetMessage (standard approach for LL hooks)
    MSG msg;
    static int checkCount = 0;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        // Log timer ticks to verify message loop is running
        if (msg.message == WM_TIMER && msg.wParam == 999) {
            checkCount++;
            FILE* f = fopen("D:\\CK\\ViKey\\app-native\\hookcheck.log", "a");
            if (f) {
                fprintf(f, "Check #%d: hook=%d\n", checkCount,
                        KeyboardHook::Instance().IsActive() ? 1 : 0);
                fclose(f);
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    CleanupInstance();
    if (hMutex) CloseHandle(hMutex);

    return static_cast<int>(msg.wParam);
}

bool InitInstance(HINSTANCE hInstance) {
    g_hInstance = hInstance;

    // Register window class
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = WINDOW_CLASS;

    if (!RegisterClassExW(&wcex)) {
        return false;
    }

    // Create hidden window (for message handling)
    // Using a regular window (not HWND_MESSAGE) for better compatibility with low-level hooks
    g_hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW,  // Don't show in taskbar
        WINDOW_CLASS,
        APP_NAME,
        WS_POPUP,          // No border, no title bar
        -32000, -32000,    // Off-screen position
        0, 0,              // Zero size
        nullptr,           // No parent (not HWND_MESSAGE)
        nullptr,
        hInstance,
        nullptr);
    if (!g_hWnd) {
        return false;
    }

    // Load settings
    Settings::Instance().Load();

    // Initialize IME processor
    if (!ImeProcessor::Instance().Initialize()) {
        MessageBoxW(nullptr, L"Failed to load core.dll", APP_NAME, MB_ICONERROR);
        return false;
    }

    // Apply settings to processor
    ImeProcessor::Instance().ApplySettings();

    // Initialize tray icon
    TrayIcon& tray = TrayIcon::Instance();
    tray.Initialize(g_hWnd, hInstance);

    // Set up tray callbacks
    tray.onToggleEnabled = []() {
        ImeProcessor::Instance().ToggleEnabled();
        Settings::Instance().enabled = ImeProcessor::Instance().IsEnabled();
        Settings::Instance().Save();
        UpdateUI();
    };

    tray.onSetMethod = [](InputMethod method) {
        ImeProcessor::Instance().SetMethod(method);
        Settings::Instance().method = method;
        Settings::Instance().Save();
        UpdateUI();
    };

    tray.onSettings = []() {
        ShowSettingsDialog();
    };

    tray.onAbout = []() {
        ShowAboutDialog();
    };

    tray.onExit = []() {
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);
    };

    // Register global hotkey (Ctrl+Space)
    HotkeyManager& hotkey = HotkeyManager::Instance();
    hotkey.Register(g_hWnd);
    hotkey.SetCallback([]() {
        ImeProcessor::Instance().ToggleEnabled();
        Settings::Instance().enabled = ImeProcessor::Instance().IsEnabled();
        Settings::Instance().Save();
        UpdateUI();
    });

    // Start IME processor
    ImeProcessor::Instance().Start();

    // Update UI state
    UpdateUI();

    // Show startup notification if not silent
    if (!Settings::Instance().silentStartup) {
        const wchar_t* lang = Settings::Instance().enabled ? L"Ti\u1EBFng Vi\u1EC7t" : L"Ti\u1EBFng Anh";
        wchar_t msg[128];
        swprintf_s(msg, L"\u0110ang ch\u1EA1y \u1EDF ch\u1EBF \u0111\u1ED9 %s\nCtrl+Space \u0111\u1EC3 chuy\u1EC3n", lang);
        tray.ShowBalloon(L"B\u1ED9 g\u00F5 ti\u1EBFng Vi\u1EC7t", msg);
    }

    return true;
}

void CleanupInstance() {
    // Stop processor
    ImeProcessor::Instance().Stop();

    // Unregister hotkey
    if (g_hWnd) {
        HotkeyManager::Instance().Unregister(g_hWnd);
    }

    // Remove tray icon
    TrayIcon::Instance().Shutdown();

    // Shutdown GDI+
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
    }
}

void UpdateUI() {
    bool enabled = ImeProcessor::Instance().IsEnabled();
    InputMethod method = ImeProcessor::Instance().GetMethod();

    TrayIcon& tray = TrayIcon::Instance();
    tray.UpdateIcon(enabled);
    tray.UpdateTooltip(enabled, method);
    tray.UpdateMenu(enabled, method);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_TRAYICON_MSG:
        TrayIcon::Instance().ProcessTrayMessage(hWnd, wParam, lParam);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_TOGGLE_ENABLED:
            if (TrayIcon::Instance().onToggleEnabled)
                TrayIcon::Instance().onToggleEnabled();
            break;

        case IDM_METHOD_TELEX:
            if (TrayIcon::Instance().onSetMethod)
                TrayIcon::Instance().onSetMethod(InputMethod::Telex);
            break;

        case IDM_METHOD_VNI:
            if (TrayIcon::Instance().onSetMethod)
                TrayIcon::Instance().onSetMethod(InputMethod::VNI);
            break;

        case IDM_SETTINGS:
            if (TrayIcon::Instance().onSettings)
                TrayIcon::Instance().onSettings();
            break;

        case IDM_ABOUT:
            if (TrayIcon::Instance().onAbout)
                TrayIcon::Instance().onAbout();
            break;

        case IDM_EXIT:
            if (TrayIcon::Instance().onExit)
                TrayIcon::Instance().onExit();
            break;
        }
        return 0;

    case WM_HOTKEY:
        HotkeyManager::Instance().ProcessHotkey(wParam);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
}

void ShowSettingsDialog() {
    DialogBoxW(g_hInstance, MAKEINTRESOURCEW(IDD_SETTINGS), g_hWnd, SettingsDialogProc);
}

void ShowAboutDialog() {
    DialogBoxW(g_hInstance, MAKEINTRESOURCEW(IDD_ABOUT), g_hWnd, AboutDialogProc);
}

// Settings dialog helper functions
void InitSettingsDialog(HWND hDlg) {
    Settings& settings = Settings::Instance();

    // Fix Vietnamese text (resource file encoding workaround)
    SetWindowTextW(hDlg, L"C\u00E0i \u0111\u1EB7t");  // Cài đặt
    SetDlgItemTextW(hDlg, IDC_CHECK_MODERN, L"B\u1ECF d\u1EA5u ki\u1EC3u m\u1EDBi");  // Bỏ dấu kiểu mới
    SetDlgItemTextW(hDlg, IDC_CHECK_AUTORESTORE, L"T\u1EF1 \u0111\u1ED9ng kh\u00F4i ph\u1EE5c ti\u1EBFng Anh");
    SetDlgItemTextW(hDlg, IDC_CHECK_AUTOCAP, L"T\u1EF1 \u0111\u1ED9ng vi\u1EBFt hoa");
    SetDlgItemTextW(hDlg, IDC_CHECK_ESCRESTORE, L"ESC kh\u00F4i ph\u1EE5c ASCII");
    SetDlgItemTextW(hDlg, IDC_CHECK_FREETONE, L"B\u1ECF d\u1EA5u t\u1EF1 do");
    SetDlgItemTextW(hDlg, IDC_CHECK_SKIPW, L"B\u1ECF qua ph\u00EDm t\u1EAFt w");
    SetDlgItemTextW(hDlg, IDC_CHECK_BRACKET, L"D\u1EA5u ngo\u1EB7c l\u00E0m ph\u00EDm t\u1EAFt");
    SetDlgItemTextW(hDlg, IDC_CHECK_SLOWMODE, L"Ch\u1EBF \u0111\u1ED9 ch\u1EADm (terminal)");
    SetDlgItemTextW(hDlg, IDC_CHECK_AUTOSTART, L"Kh\u1EDFi \u0111\u1ED9ng c\u00F9ng Windows");
    SetDlgItemTextW(hDlg, IDC_CHECK_SILENT, L"\u1EA8n khi kh\u1EDFi \u0111\u1ED9ng");
    SetDlgItemTextW(hDlg, IDC_BTN_ADD, L"Th\u00EAm");
    SetDlgItemTextW(hDlg, IDC_BTN_REMOVE, L"Xo\u00E1");
    SetDlgItemTextW(hDlg, IDC_BTN_OK, L"L\u01B0u");
    SetDlgItemTextW(hDlg, IDC_BTN_CANCEL, L"Hu\u1EF7");

    // Input method
    CheckRadioButton(hDlg, IDC_RADIO_TELEX, IDC_RADIO_VNI,
                     settings.method == InputMethod::Telex ? IDC_RADIO_TELEX : IDC_RADIO_VNI);

    // Checkboxes
    CheckDlgButton(hDlg, IDC_CHECK_MODERN, settings.modernTone ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_AUTORESTORE, settings.englishAutoRestore ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_AUTOCAP, settings.autoCapitalize ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_ESCRESTORE, settings.escRestore ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_FREETONE, settings.freeTone ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_SKIPW, settings.skipWShortcut ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_BRACKET, settings.bracketShortcut ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_SLOWMODE, settings.slowMode ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_AUTOSTART, settings.autoStart ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_SILENT, settings.silentStartup ? BST_CHECKED : BST_UNCHECKED);

    // Hotkey configuration
    CheckDlgButton(hDlg, IDC_CHECK_HOTKEY_CTRL, settings.toggleHotkey.ctrl ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_HOTKEY_SHIFT, settings.toggleHotkey.shift ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_HOTKEY_ALT, settings.toggleHotkey.alt ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_HOTKEY_WIN, settings.toggleHotkey.win ? BST_CHECKED : BST_UNCHECKED);

    // Set hotkey character (convert VK code to displayable text)
    wchar_t hotkeyChar[16] = {0};
    if (settings.toggleHotkey.vkCode == VK_SPACE) {
        wcscpy_s(hotkeyChar, L"Space");
    } else if (settings.toggleHotkey.vkCode >= 0x41 && settings.toggleHotkey.vkCode <= 0x5A) {
        hotkeyChar[0] = static_cast<wchar_t>(settings.toggleHotkey.vkCode);
    } else if (settings.toggleHotkey.vkCode >= 0x30 && settings.toggleHotkey.vkCode <= 0x39) {
        hotkeyChar[0] = static_cast<wchar_t>(settings.toggleHotkey.vkCode);
    }
    SetDlgItemTextW(hDlg, IDC_EDIT_HOTKEY, hotkeyChar);

    // Initialize ListView for shortcuts
    HWND hList = GetDlgItem(hDlg, IDC_LIST_SHORTCUTS);

    // Set extended styles
    ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // Add columns
    LVCOLUMNW col = {};
    col.mask = LVCF_TEXT | LVCF_WIDTH;

    col.pszText = (LPWSTR)L"T\u1EAFt";
    col.cx = 80;
    ListView_InsertColumn(hList, 0, &col);

    col.pszText = (LPWSTR)L"\u0110\u1EA7y \u0111\u1EE7";
    col.cx = 200;
    ListView_InsertColumn(hList, 1, &col);

    // Add shortcuts to list
    for (size_t i = 0; i < settings.shortcuts.size(); i++) {
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(i);
        item.pszText = (LPWSTR)settings.shortcuts[i].key.c_str();
        ListView_InsertItem(hList, &item);
        ListView_SetItemText(hList, static_cast<int>(i), 1, (LPWSTR)settings.shortcuts[i].value.c_str());
    }
}

void SaveSettingsFromDialog(HWND hDlg) {
    Settings& settings = Settings::Instance();

    // Input method
    settings.method = IsDlgButtonChecked(hDlg, IDC_RADIO_TELEX) == BST_CHECKED ?
                      InputMethod::Telex : InputMethod::VNI;

    // Checkboxes
    settings.modernTone = IsDlgButtonChecked(hDlg, IDC_CHECK_MODERN) == BST_CHECKED;
    settings.englishAutoRestore = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTORESTORE) == BST_CHECKED;
    settings.autoCapitalize = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTOCAP) == BST_CHECKED;
    settings.escRestore = IsDlgButtonChecked(hDlg, IDC_CHECK_ESCRESTORE) == BST_CHECKED;
    settings.freeTone = IsDlgButtonChecked(hDlg, IDC_CHECK_FREETONE) == BST_CHECKED;
    settings.skipWShortcut = IsDlgButtonChecked(hDlg, IDC_CHECK_SKIPW) == BST_CHECKED;
    settings.bracketShortcut = IsDlgButtonChecked(hDlg, IDC_CHECK_BRACKET) == BST_CHECKED;
    settings.slowMode = IsDlgButtonChecked(hDlg, IDC_CHECK_SLOWMODE) == BST_CHECKED;
    settings.autoStart = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTOSTART) == BST_CHECKED;
    settings.silentStartup = IsDlgButtonChecked(hDlg, IDC_CHECK_SILENT) == BST_CHECKED;

    // Hotkey configuration
    settings.toggleHotkey.ctrl = IsDlgButtonChecked(hDlg, IDC_CHECK_HOTKEY_CTRL) == BST_CHECKED;
    settings.toggleHotkey.shift = IsDlgButtonChecked(hDlg, IDC_CHECK_HOTKEY_SHIFT) == BST_CHECKED;
    settings.toggleHotkey.alt = IsDlgButtonChecked(hDlg, IDC_CHECK_HOTKEY_ALT) == BST_CHECKED;
    settings.toggleHotkey.win = IsDlgButtonChecked(hDlg, IDC_CHECK_HOTKEY_WIN) == BST_CHECKED;

    // Get hotkey character
    wchar_t hotkeyChar[16] = {0};
    GetDlgItemTextW(hDlg, IDC_EDIT_HOTKEY, hotkeyChar, 16);
    if (_wcsicmp(hotkeyChar, L"Space") == 0 || hotkeyChar[0] == L' ' || hotkeyChar[0] == 0) {
        settings.toggleHotkey.vkCode = VK_SPACE;
    } else if (hotkeyChar[0] >= L'A' && hotkeyChar[0] <= L'Z') {
        settings.toggleHotkey.vkCode = static_cast<UINT>(hotkeyChar[0]);
    } else if (hotkeyChar[0] >= L'a' && hotkeyChar[0] <= L'z') {
        settings.toggleHotkey.vkCode = static_cast<UINT>(hotkeyChar[0] - L'a' + L'A');
    } else if (hotkeyChar[0] >= L'0' && hotkeyChar[0] <= L'9') {
        settings.toggleHotkey.vkCode = static_cast<UINT>(hotkeyChar[0]);
    }

    // Get shortcuts from ListView
    HWND hList = GetDlgItem(hDlg, IDC_LIST_SHORTCUTS);
    int count = ListView_GetItemCount(hList);

    settings.shortcuts.clear();
    for (int i = 0; i < count; i++) {
        wchar_t key[64] = {};
        wchar_t value[256] = {};

        ListView_GetItemText(hList, i, 0, key, 64);
        ListView_GetItemText(hList, i, 1, value, 256);

        if (wcslen(key) > 0 && wcslen(value) > 0) {
            settings.shortcuts.push_back({key, value});
        }
    }

    // Save to registry
    settings.Save();

    // Apply to processor
    ImeProcessor::Instance().ApplySettings();

    // Update hotkey registration
    HotkeyManager::Instance().UpdateHotkey(g_hWnd);

    UpdateUI();
}

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        InitSettingsDialog(hDlg);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_ADD: {
            wchar_t key[64] = {};
            wchar_t value[256] = {};
            GetDlgItemTextW(hDlg, IDC_EDIT_KEY, key, 64);
            GetDlgItemTextW(hDlg, IDC_EDIT_VALUE, value, 256);

            if (wcslen(key) > 0 && wcslen(value) > 0) {
                HWND hList = GetDlgItem(hDlg, IDC_LIST_SHORTCUTS);
                int idx = ListView_GetItemCount(hList);

                LVITEMW item = {};
                item.mask = LVIF_TEXT;
                item.iItem = idx;
                item.pszText = key;
                ListView_InsertItem(hList, &item);
                ListView_SetItemText(hList, idx, 1, value);

                SetDlgItemTextW(hDlg, IDC_EDIT_KEY, L"");
                SetDlgItemTextW(hDlg, IDC_EDIT_VALUE, L"");
            }
            return TRUE;
        }

        case IDC_BTN_REMOVE: {
            HWND hList = GetDlgItem(hDlg, IDC_LIST_SHORTCUTS);
            int sel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
            if (sel >= 0) {
                ListView_DeleteItem(hList, sel);
            }
            return TRUE;
        }

        case IDOK:
            SaveSettingsFromDialog(hDlg);
            EndDialog(hDlg, IDOK);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}
