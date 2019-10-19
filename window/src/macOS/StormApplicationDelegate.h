#pragma once

#import <AppKit/NSApplication.h>

@interface StormApplicationDelegate : NSObject<NSApplicationDelegate>

	- (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*)sender;
	- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*)theApplication;

@end
