#pragma once

#include <storm/window/VideoSettings.hpp>
#include <storm/window/WindowStyle.hpp>
#include <string>

#import <AppKit/NSWindow.h>

class WindowImpl;
@interface StormWindowController: NSResponder<NSWindowDelegate>
	- (id) initWithSettings: (storm::window::VideoSettings)settings withStyle: (storm::window::WindowStyle)style withTitle: (std::string)title withRequester: (WindowImpl*)_requester;
	- (void) dealloc;

	- (void) setRequester: (WindowImpl*)requester;

	- (void) close;
	- (BOOL) isOpen;
	- (BOOL) isVisible;

	- (void) showWindow;
	- (void) hideWindow;

	- (void) processEvent;
	- (void*) nativeHandle;

	- (NSSize) size;

	- (BOOL) windowShouldClose: (id)sender;

    - (void) windowWillClose: (id)sender;
	- (NSSize) windowWillResize: (NSWindow *)sender toSize: (NSSize)frameSize;

	- (void) windowDidMiniaturize: (NSNotification *)notification;
	- (void) windowDidDeminiaturize: (NSNotification *)notification;

	- (void) setMousePosition:(NSPoint) point;
	- (void) setWindowTitle: (std::string)title;

	- (NSPoint) convertPoint: (NSPoint)point;
@end
