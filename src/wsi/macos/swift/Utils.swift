@inline(__always)
public func unreachable() -> Never {
    return unsafeBitCast((), to: Never.self)
}

@inline(__always)
public func assertUnreachable(_ message: @autoclosure () -> String = "Encountered unreachable path",
                              file: StaticString = #file,
                              line: UInt = #line) -> Never {
    var isDebug = false
    assert({ isDebug = true; return true }())

    if isDebug {
        fatalError(message(), file: file, line: line)
    } else {
        unreachable()
    }
}
