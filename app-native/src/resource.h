// ViKey - Win32 Resource IDs
// resource.h

#pragma once

// Icon IDs
#define IDI_APP_ICON          100
#define IDI_ICON_VN           101
#define IDI_ICON_EN           102

// Menu IDs
#define IDM_TRAY_MENU         200
#define IDM_TOGGLE_ENABLED    201
#define IDM_METHOD_TELEX      202
#define IDM_METHOD_VNI        203
#define IDM_SETTINGS          204
#define IDM_ABOUT             205
#define IDM_EXIT              206

// Dialog IDs
#define IDD_SETTINGS          300
#define IDD_ABOUT             301

// Settings Dialog Controls
#define IDC_CHECK_ENABLED     400
#define IDC_RADIO_TELEX       401
#define IDC_RADIO_VNI         402
#define IDC_CHECK_MODERN      403
#define IDC_CHECK_AUTORESTORE 404
#define IDC_CHECK_AUTOCAP     405
#define IDC_CHECK_ESCRESTORE  406
#define IDC_CHECK_FREETONE    407
#define IDC_CHECK_SKIPW       408
#define IDC_CHECK_BRACKET     409
#define IDC_CHECK_SLOWMODE    410
#define IDC_CHECK_AUTOSTART   411
#define IDC_CHECK_SILENT      425
#define IDC_LIST_SHORTCUTS    412
#define IDC_EDIT_KEY          413
#define IDC_EDIT_VALUE        414
#define IDC_BTN_ADD           415
#define IDC_BTN_REMOVE        416
#define IDC_BTN_OK            IDOK
#define IDC_BTN_CANCEL        IDCANCEL

// Hotkey Config Controls
#define IDC_CHECK_HOTKEY_CTRL   420
#define IDC_CHECK_HOTKEY_SHIFT  421
#define IDC_CHECK_HOTKEY_ALT    422
#define IDC_CHECK_HOTKEY_WIN    423
#define IDC_EDIT_HOTKEY         424

// Hotkey ID
#define HOTKEY_TOGGLE         500

// Timer IDs
#define TIMER_UPDATE          600

// Custom messages
#define WM_TRAYICON           (WM_USER + 1)
#define WM_TOGGLE_IME         (WM_USER + 2)

// String IDs
#define IDS_APP_TITLE         1000
#define IDS_TOOLTIP_VN        1001
#define IDS_TOOLTIP_EN        1002
