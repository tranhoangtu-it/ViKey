//
//  Settings.swift
//  ViKey macOS Input Method
//
//  Settings management via UserDefaults
//

import Foundation

/// ViKey settings manager
class Settings {
    static let shared = Settings()

    private let defaults = UserDefaults.standard

    // MARK: - Keys

    private enum Keys {
        static let inputMethod = "InputMethod"
        static let modernTone = "ModernTone"
        static let escRestore = "EscRestore"
        static let skipWShortcut = "SkipWShortcut"
        static let bracketShortcut = "BracketShortcut"
        static let freeTone = "FreeTone"
        static let englishAutoRestore = "EnglishAutoRestore"
        static let autoCapitalize = "AutoCapitalize"
        static let enabled = "Enabled"
    }

    // MARK: - Properties

    var inputMethod: InputMethod {
        get {
            InputMethod(rawValue: UInt8(defaults.integer(forKey: Keys.inputMethod))) ?? .telex
        }
        set {
            defaults.set(Int(newValue.rawValue), forKey: Keys.inputMethod)
            RustBridge.shared.setMethod(newValue)
        }
    }

    var modernTone: Bool {
        get { defaults.bool(forKey: Keys.modernTone) }
        set {
            defaults.set(newValue, forKey: Keys.modernTone)
            RustBridge.shared.setModernTone(newValue)
        }
    }

    var escRestore: Bool {
        get { defaults.bool(forKey: Keys.escRestore) }
        set {
            defaults.set(newValue, forKey: Keys.escRestore)
            RustBridge.shared.setEscRestore(newValue)
        }
    }

    var skipWShortcut: Bool {
        get { defaults.bool(forKey: Keys.skipWShortcut) }
        set {
            defaults.set(newValue, forKey: Keys.skipWShortcut)
            RustBridge.shared.setSkipWShortcut(newValue)
        }
    }

    var bracketShortcut: Bool {
        get { defaults.bool(forKey: Keys.bracketShortcut) }
        set {
            defaults.set(newValue, forKey: Keys.bracketShortcut)
            RustBridge.shared.setBracketShortcut(newValue)
        }
    }

    var freeTone: Bool {
        get { defaults.bool(forKey: Keys.freeTone) }
        set {
            defaults.set(newValue, forKey: Keys.freeTone)
            RustBridge.shared.setFreeTone(newValue)
        }
    }

    var englishAutoRestore: Bool {
        get { defaults.bool(forKey: Keys.englishAutoRestore) }
        set {
            defaults.set(newValue, forKey: Keys.englishAutoRestore)
            RustBridge.shared.setEnglishAutoRestore(newValue)
        }
    }

    var autoCapitalize: Bool {
        get { defaults.bool(forKey: Keys.autoCapitalize) }
        set {
            defaults.set(newValue, forKey: Keys.autoCapitalize)
            RustBridge.shared.setAutoCapitalize(newValue)
        }
    }

    var enabled: Bool {
        get { defaults.object(forKey: Keys.enabled) as? Bool ?? true }
        set {
            defaults.set(newValue, forKey: Keys.enabled)
            RustBridge.shared.setEnabled(newValue)
        }
    }

    // MARK: - Initialization

    private init() {
        registerDefaults()
    }

    private func registerDefaults() {
        defaults.register(defaults: [
            Keys.inputMethod: 0, // Telex
            Keys.modernTone: true,
            Keys.escRestore: true,
            Keys.skipWShortcut: false,
            Keys.bracketShortcut: false,
            Keys.freeTone: false,
            Keys.englishAutoRestore: true,
            Keys.autoCapitalize: false,
            Keys.enabled: true
        ])
    }

    /// Apply all settings to Rust bridge
    func applyAll() {
        RustBridge.shared.setMethod(inputMethod)
        RustBridge.shared.setModernTone(modernTone)
        RustBridge.shared.setEscRestore(escRestore)
        RustBridge.shared.setSkipWShortcut(skipWShortcut)
        RustBridge.shared.setBracketShortcut(bracketShortcut)
        RustBridge.shared.setFreeTone(freeTone)
        RustBridge.shared.setEnglishAutoRestore(englishAutoRestore)
        RustBridge.shared.setAutoCapitalize(autoCapitalize)
        RustBridge.shared.setEnabled(enabled)
    }
}
