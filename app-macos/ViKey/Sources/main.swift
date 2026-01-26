//
//  main.swift
//  ViKey macOS Input Method
//
//  Entry point for Input Method bundle
//

import InputMethodKit
import AppKit

// Connection name for IMKit server
let kConnectionName = "ViKey_Connection"

// Create and run the input method server
autoreleasepool {
    // Get bundle identifier
    guard let bundleId = Bundle.main.bundleIdentifier else {
        NSLog("ViKey: Failed to get bundle identifier")
        exit(1)
    }

    // Create IMK server
    guard let server = IMKServer(name: kConnectionName, bundleIdentifier: bundleId) else {
        NSLog("ViKey: Failed to create IMKServer")
        exit(1)
    }

    NSLog("ViKey: Input method server started")

    // Run the application
    NSApplication.shared.run()
}
