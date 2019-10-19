#pragma once

#import <AppKit/NSWindow.h>

@interface StormWindow : NSWindow
	- (BOOL) acceptsFirstResponder;
	- (BOOL) canBecomeKeyWindow;
	- (void) performClose: (id)sender;
@end
