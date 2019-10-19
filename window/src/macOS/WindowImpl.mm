#include <string>

#import <AppKit/NSScreen.h>

#import "WindowImpl.h"
#import "StormWindowController.h"
#import "AutoReleasePoolWrapper.h"
#import "StormApplication.h"
#import "StormApplicationDelegate.h"
#import "Utils.h"

using namespace storm::window;

static bool is_process_set_as_application = false;

WindowImpl::WindowImpl() noexcept : AbstractWindow(), m_controller(nil) {
	ensureThreadHasPool();

	initCocoaProcess();
}

WindowImpl::WindowImpl(const std::string &title, const VideoSettings &settings, WindowStyle style) noexcept
	: WindowImpl() {
	create(title, settings, style);
}

WindowImpl::~WindowImpl() {
	[m_controller close];
	[m_controller release];

	drainThreadPool();
}

//FIXME make invalid context
void WindowImpl::create(const std::string &title, const VideoSettings &settings, WindowStyle style) noexcept {
	initCocoaProcess();

	m_title            = title;
	m_video_settings   = settings;
	m_style            = style;

	m_controller = [[StormWindowController alloc] initWithSettings: settings withStyle: style withTitle: title withRequester: this];
}

void WindowImpl::close() noexcept {
	[m_controller close];

	drainThreadPool();
}

void WindowImpl::display() noexcept {

}

bool WindowImpl::pollEvent(storm::window::Event &event, [[maybe_unused]] void *native_event) noexcept {
	[m_controller processEvent];

	return AbstractWindow::pollEvent(event);
}

bool WindowImpl::waitEvent(storm::window::Event &event, [[maybe_unused]] void *native_event) noexcept {;
	while(!AbstractWindow::waitEvent(event, native_event))
		[m_controller processEvent];
	
	return true;
}

void WindowImpl::setTitle(const std::string &title) noexcept {
	m_title = title;

	[m_controller setWindowTitle: title];
}

void WindowImpl::setVideoSettings(const VideoSettings &settings) noexcept {
	m_video_settings = settings;
}

storm::core::Extent WindowImpl::size() const noexcept {
	auto size = [m_controller size];

	return toStormVec(size);
}

bool WindowImpl::isOpen() const noexcept {
	return [m_controller isOpen] == YES;
}

bool WindowImpl::isVisible() const noexcept {
	return [m_controller isVisible] == YES;
}

storm::window::NativeHandle WindowImpl::nativeHandle() const noexcept {
	return [m_controller nativeHandle];
}

void WindowImpl::initCocoaProcess() {
	if(!is_process_set_as_application) {
		[StormApplication sharedApplication];

		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
		[NSApp activateIgnoringOtherApps: YES];

		if(![[StormApplication sharedApplication] delegate])
			[[StormApplication sharedApplication] setDelegate: [[StormApplicationDelegate alloc] init]];

		[[StormApplication sharedApplication] finishLaunching];

		is_process_set_as_application = true;
	}
}
