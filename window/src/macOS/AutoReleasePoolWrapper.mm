#import "AutoReleasePoolWrapper.h"

#import <Foundation/NSAutoreleasePool.h>

thread_local NSAutoreleasePool* pool = nullptr;

void createNewPool() {
	pool = [[NSAutoreleasePool alloc] init];
}

void destroyPool() {
	[pool drain];
}

extern "C" void ensureThreadHasPool() {
	if(pool == nullptr) {
		createNewPool();
	}
}

extern "C" void drainThreadPool() {
	destroyPool();
	createNewPool();
}
