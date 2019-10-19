#import "StormApplication.h"

@implementation StormApplication

+ (void) processEvent {
	[StormApplication sharedApplication];

	NSEvent *event = nil;

	do {
		event = [NSApp nextEventMatchingMask: NSEventMaskAny
					   untilDate: [NSDate distantPast]
					   inMode: NSDefaultRunLoopMode
					   dequeue: YES];

		[NSApp sendEvent:event];
	} while(event != nil);
}

@end
