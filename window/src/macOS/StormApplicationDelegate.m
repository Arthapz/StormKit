#import "StormApplicationDelegate.h"

@implementation StormApplicationDelegate

- (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*)sender {
	return NSTerminateCancel;
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*)theApplication {
	return YES;
}

@end
