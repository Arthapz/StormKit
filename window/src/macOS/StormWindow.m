#import "StormWindow.h"
#import "StormApplication.h"

@implementation  StormWindow

- (BOOL) acceptsFirstResponder {
	return YES;
}

- (BOOL) canBecomeKeyWindow {
	return YES;
}

- (void) performClose: (id)sender {
	[self close];
}

@end
