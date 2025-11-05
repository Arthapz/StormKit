import AppKit

final class AppDelegate: NSResponder, NSApplicationDelegate {
  override init() {
    super.init()
  }

  deinit {
  }

  required init?(coder aCoder: NSCoder) {
    fatalError("Swift: init(coder:) not implemented!")
  }

  func applicationDidFinishLaunching(_ notification: Notification) {
  }

  func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
    .terminateCancel
  }

  func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
    false
  }
}

public func processEvents() {
    var event: NSEvent? = nil

    repeat {
      if let event {
        NSApplication.shared.sendEvent(event)
      }
      event = NSApplication.shared.nextEvent(matching: [.any], until: NSDate.distantPast, inMode: .default, dequeue: true)
    } while event != nil
}

public func initCocoaProcess() {
  let app = NSApplication.shared

  if app.delegate == nil {
    NSApp.setActivationPolicy(.regular)
    NSApp.activate(ignoringOtherApps: true)

    let delegate = AppDelegate()
    app.delegate = delegate

    app.finishLaunching()
    processEvents()
  }
}
