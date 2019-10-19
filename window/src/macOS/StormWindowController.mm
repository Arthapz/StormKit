#import "StormWindowController.h"
#import "AutoReleasePoolWrapper.h"
#import "StormView.h"
#import "StormWindow.h"
#import "StormApplication.h"
#include "WindowImpl.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

NSString *fromStdString(const std::string &str) {
	return [NSString stringWithCString: str.c_str()
										encoding:[NSString defaultCStringEncoding]];
}

@implementation StormWindowController {
	WindowImpl*        requester;
	NSWindow*          window;
	StormView*   view;
	BOOL               is_open;
}

- (id) initWithSettings: (storm::window::VideoSettings)settings withStyle: (storm::window::WindowStyle)style withTitle: (std::string)title withRequester: (WindowImpl*)_requester {
	using namespace storm::window;

	[super init];

	if(self) {
		view      = nil;
		window    = nil;

		auto fullscreen = (style & WindowStyle::Fullscreen) == WindowStyle::Fullscreen;

		auto window_style = NSUInteger(NSWindowStyleMaskTitled);
		if((style & WindowStyle::Close) == WindowStyle::Close)
			window_style |= NSWindowStyleMaskClosable;

		if((style & WindowStyle::Resizable) == WindowStyle::Resizable)
			window_style |= NSWindowStyleMaskResizable;

		if((style & WindowStyle::Minimizable) == WindowStyle::Minimizable)
			window_style |= NSWindowStyleMaskMiniaturizable;

		const auto rect = NSMakeRect(0, 0, settings.size.width, settings.size.height);
		window = [[StormWindow alloc] initWithContentRect: rect
									  styleMask: window_style
									  backing: NSBackingStoreBuffered
									  defer: NO];

		auto frame = [window convertRectToBacking: [[window contentView] frame]];

		view   = [[StormView alloc] initWithFrame: frame withRequester: _requester];
		[view setLayer:[CAMetalLayer layer]];
		[view setWantsLayer:YES];
		
		[window setReleasedWhenClosed:NO];
		[window makeFirstResponder: self];

		[window setContentView: view];
		[window setOpaque: YES];
		[window setTitle: fromStdString(title)];

		[window setDelegate: self];
		[window center];
		[window setAutodisplay:YES];
		[window orderFrontRegardless];

		if(fullscreen)
            [window toggleFullScreen: self];

		requester = _requester;

		is_open = YES;
	}

	return self;
}

- (void) dealloc {
	[self close];

	[window release];
	[view release];

	[super dealloc];
}

- (void) setRequester: (WindowImpl*)_requester {
	requester = _requester;
	[view setRequester: requester];
}

- (void) close {
	[window close];
	[window setDelegate: nil];

	[self setRequester: nullptr];
}

- (BOOL) isOpen {
	return is_open;
}

- (BOOL) isVisible {
	return [window isVisible];
}

- (void) showWindow {
	[window orderOut: nil];
}

- (void) hideWindow {
	[window makeKeyAndOrderFront: nil];
}

- (void) processEvent {
	[StormApplication processEvent];
	drainThreadPool();

	//CGAssociateMouseAndMouseCursorPosition(TRUE);
}

- (void*) nativeHandle {
	return view;
}

- (NSSize) size {
	return [window contentRectForFrameRect: window.frame].size;
}

- (BOOL) windowShouldClose: (id)sender {
	is_open = NO;
	return YES;
}

- (void) windowWillClose: (id)sender {
	requester->closeEvent();
}

- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize {
	requester->resizeEvent(frameSize.width, frameSize.height);

	return frameSize;
}

- (void) windowDidMiniaturize: (NSNotification *)notification {
	requester->minimizeEvent();
}

- (void) windowDidDeminiaturize: (NSNotification *)notification {
	requester->maximizeEvent();
}

- (void) setMousePosition:(NSPoint) point {
	[view setMousePosition: point];
}

- (void) setWindowTitle: (std::string)title {
	[window setTitle: [NSString stringWithCString: title.c_str() encoding:[NSString defaultCStringEncoding]]];
}

- (NSPoint) convertPoint: (NSPoint)point {
	NSPoint pointInView = [view convertPoint: point fromView: nil];

	return pointInView;
}

@end
