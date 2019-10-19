#import "StormView.h"
#import "WindowImpl.h"
#include "Utils.h"

#import <AppKit/NSTrackingArea.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSScreen.h>

@implementation StormView  {
	NSTrackingArea*    trackingArea;
	WindowImpl*        requester;
	BOOL               isMouseInside;
	void*              nativeEvent;
}

- (BOOL) acceptsFirstResponder {
	return YES;
}

- (id) initWithFrame: (NSRect) frame withRequester: (WindowImpl*)_requester {
	self = [super initWithFrame: frame];

	isMouseInside = NO;
	nativeEvent = nullptr;
	requester = _requester;

	return self;
}

- (void) setRequester: (WindowImpl*)_requester {
	requester = _requester;
}

- (void) mouseDown:(NSEvent *)event {
	/*if(nativeEvent)
		*reinterpret_cast<NSEvent*>(nativeEvent) = *event;*/

	[self handleMouseDown: event];

	[[self nextResponder] mouseDown: event];
}

- (void) rightMouseDown:(NSEvent *)event {
	[self handleMouseDown: event];

	[[self nextResponder] mouseDown: event];
}

- (void) otherMouseDown:(NSEvent *)event {
	[self handleMouseDown: event];

	[[self nextResponder] mouseDown: event];
}

- (void) handleMouseDown:(NSEvent *)event {
	auto mouse_key = toStormMouseButton([event buttonNumber]);

	requester->mouseDownEvent(mouse_key, event.locationInWindow.x, event.locationInWindow.y);
}

- (void) mouseUp:(NSEvent *)event {
	[self handleMouseUp: event];

	[[self nextResponder] mouseUp: event];
}

- (void) rightMouseUp:(NSEvent *)event {
	[self handleMouseUp: event];

	[[self nextResponder] mouseUp: event];
}

- (void) otherMouseUp:(NSEvent *)event {
	[self handleMouseUp: event];

	[[self nextResponder] mouseUp: event];
}

- (void) handleMouseUp:(NSEvent *)event {
	auto mouse_key = toStormMouseButton([event buttonNumber]);

	requester->mouseUpEvent(mouse_key, event.locationInWindow.x, event.locationInWindow.y);
}

- (void) mouseMoved:(NSEvent*)event {
	[self handleMouseMove:event];

	[[self nextResponder] mouseMoved:event];
}

- (void) mouseDragged:(NSEvent*)event {
	[self handleMouseMove:event];
	
	[[self nextResponder] mouseDragged:event];
}

- (void) rightMouseDragged:(NSEvent*)event {
	[self handleMouseMove:event];

	[[self nextResponder] rightMouseDragged:event];
}

- (void) handleMouseMove:(NSEvent*)event {
	if(isMouseInside) {
		auto point = toStormVec(event.locationInWindow);

		point.y = requester->videoSettings().size.h - point.y;

		requester->mouseMoveEvent(point.x, point.y);
	}
}

- (void) mouseEntered:(NSEvent*)event {
	isMouseInside = YES;

	requester->mouseEnteredEvent();
}

- (void) mouseExited:(NSEvent*)event {
	isMouseInside = NO;

	requester->mouseExitedEvent();
}

- (void) keyDown:(NSEvent *)event {
	auto string = [event charactersIgnoringModifiers];
	auto key = storm::window::Key::UNKNOW;

	if([string length] > 0)
		key = localizedKeyToStormKey([string characterAtIndex:0]);

	if(key == storm::window::Key::UNKNOW)
		key = nonLocalizedKeytoStormKey(event.keyCode);

	requester->keyDownEvent(key);
}

- (void) keyUp:(NSEvent *)event {
	auto string = [event charactersIgnoringModifiers];
	auto key = storm::window::Key::UNKNOW;

	if([string length] > 0)
		key = localizedKeyToStormKey([string characterAtIndex:0]);

	if(key == storm::window::Key::UNKNOW)
		key = nonLocalizedKeytoStormKey(event.keyCode);

	requester->keyUpEvent(key);
}

- (void) updateTrackingAreas {
	[super updateTrackingAreas];
	if(trackingArea != nil) {
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}

	NSUInteger opts = (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways | NSTrackingMouseMoved | NSTrackingEnabledDuringMouseDrag);
	trackingArea = [ [NSTrackingArea alloc] initWithRect:[self bounds]
												 options:opts
												   owner:self
												userInfo:nil];
	[self addTrackingArea:trackingArea];
}

-(NSPoint) convertPointToScreen: (NSPoint) point
{
	NSRect rect = NSZeroRect;
	rect.origin = point;
	rect = [[self window] convertRectToScreen:rect];
	return rect.origin;
}

- (NSPoint) relativeToGlobal: (NSPoint) point {
	point.y = [self frame].size.height - point.y;

	point = [self convertPoint:point toView:self];
	point = [self convertPoint:point toView:nil];

	point = [self convertPointToScreen:point];

	const float screenHeight = [[[self window] screen] frame].size.height;
	point.y = screenHeight - point.y;

	return point;
}

-(CGDirectDisplayID) displayId {
	NSScreen* screen = [[self window] screen];
	NSNumber* displayId = [[screen deviceDescription] objectForKey:@"NSScreenNumber"];

	return [displayId intValue];
}

- (void) setNativeEventRetriever: (void*) native_event {
	nativeEvent = native_event;
}


@end
