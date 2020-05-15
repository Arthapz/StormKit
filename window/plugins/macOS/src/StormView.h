#pragma once

#import <AppKit/NSView.h>
#include <storm/core/Platform.hpp>

class WindowImpl;
@interface StormView : NSView

	- (BOOL) acceptsFirstResponder;
	- (id) initWithFrame: (NSRect) frame withRequester: (WindowImpl*)_requester withWindow: (NSWindow*)window;

	- (NSWindow*) myWindow;
	- (void) mouseDown:(NSEvent *)event;
	- (void) rightMouseDown:(NSEvent *)event;
	- (void) otherMouseDown:(NSEvent *)event;
	- (void) handleMouseDown:(NSEvent *)event;

	- (void) mouseUp:(NSEvent *)event;
	- (void) rightMouseUp:(NSEvent *)event;
	- (void) otherMouseUp:(NSEvent *)event;
	- (void) handleMouseUp:(NSEvent *)event;

	- (void) mouseMoved:(NSEvent*)event;
	- (void) mouseDragged:(NSEvent*)event;
	- (void) rightMouseDragged: (NSEvent*)event;
	- (void) handleMouseMove:(NSEvent*)event;

	- (void) mouseEntered:(NSEvent*)event;
	- (void) mouseExited:(NSEvent*)event;

	- (void) keyDown:(NSEvent *)event;
	- (void) keyUp:(NSEvent *)event;

	- (void) updateTrackingAreas;

	- (void) setRequester: (WindowImpl*)_requester;

	- (NSPoint) relativeToGlobal: (NSPoint) point;
	- (void) setNativeEventRetriever: (void*) native_event;
@end
