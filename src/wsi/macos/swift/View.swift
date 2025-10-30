import AppKit
import Carbon
import SwiftStormKitCpp

class View: NSView {
    var bitmap: CGImage?
    var rect: CGRect
    var id: WindowID
    override var acceptsFirstResponder: Bool { return true }

    init(frame frameRect: NSRect, id init_id: WindowID) {
        id = init_id
        rect = CGRect(x: 0, y: 0, width: 0, height: 0)
        super.init(frame: frameRect)
    }

    required init?(coder aCoder: NSCoder) {
        assertUnreachable("Unreachable")
    }

    func updateID(id new_id: WindowID) {
        id = new_id
    }

    override func updateTrackingAreas() {
        super.updateTrackingAreas()

        let opts: NSTrackingArea.Options = [.mouseEnteredAndExited, .activeInActiveApp, .mouseMoved, .enabledDuringMouseDrag]
        let trackingArea = NSTrackingArea(rect: bounds, options: opts, owner: self)
        addTrackingArea(trackingArea)
    }

    override func mouseDown(with event: NSEvent) {
        swiftMouseDownEvent(id, event.buttonNumber.c, event.locationInWindow.x.ci, frame.height.ci - event.locationInWindow.y.ci)
    }

    override func mouseUp(with event: NSEvent) {
        swiftMouseUpEvent(id, event.buttonNumber.c, event.locationInWindow.x.ci, frame.height.ci - event.locationInWindow.y.ci)
    }

    override func rightMouseDown(with event: NSEvent) {
        swiftMouseDownEvent(id, event.buttonNumber.c, event.locationInWindow.x.ci, frame.height.ci - event.locationInWindow.y.ci)
    }

    override func rightMouseUp(with event: NSEvent) {
        swiftMouseUpEvent(id, event.buttonNumber.c, event.locationInWindow.x.ci, frame.height.ci - event.locationInWindow.y.ci)
    }

    override func otherMouseDown(with event: NSEvent) {
        swiftMouseDownEvent(id, event.buttonNumber.c, event.locationInWindow.x.ci, frame.height.ci - event.locationInWindow.y.ci)
    }

    override func otherMouseUp(with event: NSEvent) {
        swiftMouseUpEvent(id, event.buttonNumber.c, event.locationInWindow.x.ci, frame.height.ci - event.locationInWindow.y.ci)
    }

    override func mouseMoved(with event: NSEvent) {
        swiftMouseMovedEvent(id, event.locationInWindow.x.ci, event.locationInWindow.x.ci)
    }

    override func keyDown(with event: NSEvent) {
        var c = CChar()
        if let string = event.characters, !string.isEmpty {
            c = string.utf8CString[0]
        }
        swiftKeyDownEvent(id, event.keyCode, c)
    }

    override func keyUp(with event: NSEvent) {
        var c = CChar()
        if let string = event.characters, !string.isEmpty {
            c = string.utf8CString[0]
        }
        swiftKeyUpEvent(id, event.keyCode, c)
    }

    func drawBitmap(data: UnsafeMutablePointer<CUnsignedChar>, frame: NSRect, offset_y: CGFloat) {
        guard wantsLayer != false else {
            let colorSpace = CGColorSpaceCreateDeviceRGB()
            let width = frame.width
            let height = frame.height - offset_y
            let bitmapContext = CGContext.init(
                data: data,
                width: width.i,
                height: height.i,
                bitsPerComponent: 8,
                bytesPerRow: (4 * width).i,
                space: colorSpace,
                bitmapInfo: CGImageAlphaInfo.noneSkipLast.rawValue
            )
            if let bitmapContext {
                if let _bitmap = bitmapContext.makeImage() {
                    bitmap = _bitmap
                    rect = CGRect(x: 0, y: 0, width: width, height: height)
                } else {
                    print("Failed to create bitmap image, can't fill framebuffer")
                }
            } else {
                print("Failed to create bitmap context, can't fill framebuffer")
            }
            updateLayer()
            return
        }
    }

    override func draw(_ dirtyRect: NSRect) {
        guard wantsLayer != false else {
            if let bitmap {
                if let drawContext = NSGraphicsContext.current?.cgContext {
                    drawContext.draw(bitmap, in: rect)
                } else {
                    print("Failed to get draw context, can't fill framebuffer")
                }
            }
            return
        }
    }
}
