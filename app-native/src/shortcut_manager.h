// ViKey - Shortcut Manager
// shortcut_manager.h
// Manages text shortcuts (abbreviations) expansion

#pragma once

#include <string>
#include <vector>
#include <utility>

// Text shortcut definition
struct TextShortcut {
    std::wstring key;
    std::wstring value;
};

class ShortcutManager {
public:
    static ShortcutManager& Instance();

    // Update the shortcuts list from settings
    void SetShortcuts(const std::vector<TextShortcut>& shortcuts);

    // Get current shortcuts
    const std::vector<TextShortcut>& GetShortcuts() const { return m_shortcuts; }

    // Process a character being typed
    void OnChar(char c);

    // Called when backspace is pressed
    void OnBackspace();

    // Clear the buffer (on word boundary, Ctrl, etc.)
    void Clear();

    // Check if current buffer matches a shortcut (call on Space)
    // Returns (expansion, bufferLength) or (empty, 0) if no match
    std::pair<std::wstring, size_t> CheckExpansion();

    // Get current buffer (for debugging)
    const std::string& CurrentBuffer() const { return m_buffer; }

private:
    ShortcutManager();
    ~ShortcutManager() = default;
    ShortcutManager(const ShortcutManager&) = delete;
    ShortcutManager& operator=(const ShortcutManager&) = delete;

    std::vector<TextShortcut> m_shortcuts;
    std::string m_buffer;
    static constexpr size_t MAX_BUFFER_LENGTH = 50;
};
