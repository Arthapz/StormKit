extern int macosMain(int argc, char **argv);

#import <Foundation/NSFileManager.h>
#import <Foundation/NSBundle.h>

int main(int argc, char **argv) {
	[[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];

	return macosMain(argc, argv);
}
