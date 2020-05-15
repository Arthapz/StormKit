#import "InputHandlerImpl.h"
#import "StormView.h"
#import "Utils.h"

#import <AppKit/AppKit.h>

#include <storm/window/Event.hpp>
#include <storm/window/Window.hpp>
#include <storm/window/VideoSettings.hpp>

#include <cmath>

using namespace storm;
using namespace storm::window;

void setMousePositionInternal(CGPoint point) {
	auto move_event = CGEventCreateMouseEvent(
		nullptr,
		kCGEventMouseMoved,
		std::move(point),
		kCGMouseButtonLeft
		);

	CGEventPost(kCGSessionEventTap, move_event);
	CFRelease(move_event);
}

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl() {

}

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::~InputHandlerImpl() = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl(InputHandlerImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl &InputHandlerImpl::operator=(InputHandlerImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isKeyPressed(Key key) {
	initIOHID();

	auto elements = m_keys[static_cast<core::ArraySize>(key)];

	auto state = false;
	for(auto element = std::begin(elements); element != std::end(elements);) {
		auto value = IOHIDValueRef{nullptr};
		auto device = IOHIDElementGetDevice(*element);

		IOHIDDeviceGetValue(device, *element, &value);

		if (!value) {
			CFRelease(*element);
			element = elements.erase(element);
		} else if (IOHIDValueGetIntegerValue(value) == 1) {
			state = true;
			break;
		} else ++element;
	}

	return state;
}

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isMouseButtonPressed(MouseButton button) {
	auto state = [NSEvent pressedMouseButtons];

	switch(button) {
	case MouseButton::LEFT:
		return (state & (1 << 0)) != 0;
	case MouseButton::RIGHT:
		return (state & (1 << 1)) != 0;
	case MouseButton::MIDDLE:
		return (state & (1 << 2)) != 0;
	case MouseButton::BUTTON1:
		return (state & (1 << 3)) != 0;
	case MouseButton::BUTTON2:
		return (state & (1 << 4)) != 0;
	default:
		return false;
	}

	return false;
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePosition(core::Position2u position) {
	const auto scale = [[NSScreen mainScreen] backingScaleFactor];
	const auto point = CGPoint{gsl::narrow_cast<double>(position->x / scale), gsl::narrow_cast<double>(position->y / scale)};

	setMousePositionInternal(std::move(point));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePosition(core::Position2i position, const Window &relative_to) {
	auto native_handle = static_cast<StormView*>(relative_to.nativeHandle());
	const auto scale = [[NSScreen mainScreen] backingScaleFactor];

	const auto point = CGPoint{gsl::narrow_cast<double>(position->x / scale), gsl::narrow_cast<double>(position->y / scale)};
	const auto on_screen = [native_handle relativeToGlobal:point];

	setMousePositionInternal(std::move(on_screen));
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandlerImpl::getMousePosition() {
	const auto scale = [[NSScreen mainScreen] backingScaleFactor];
	const auto desktop_height = VideoSettings::getDesktopModes()[0].size.height;
	const auto pos = [NSEvent mouseLocation];

	const auto position = core::Vector2u{scale * pos.x, desktop_height - (scale * pos.y)};

	return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2i InputHandlerImpl::getMousePosition(const Window &relative_to) {
	auto native_handle = static_cast<StormView*>(relative_to.nativeHandle());
	auto window = [native_handle myWindow];
	const auto scale = [[NSScreen mainScreen] backingScaleFactor];

	const auto window_height = relative_to.size().height;

	const auto screen_pos = [NSEvent mouseLocation];

	const auto rect = NSMakeRect(screen_pos.x, screen_pos.y, 0, 0);
	const auto window_rect = [window convertRectFromScreen: rect];

	const NSPoint pos = 
		[native_handle convertPoint: NSMakePoint(window_rect.origin.x, window_rect.origin.y) fromView: native_handle];
	const auto position = core::Vector2i{scale * pos.x, scale * (window_height - pos.y)};

	return core::makeNamed<core::Position2i>(std::move(position));
}


/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setVirtualKeyboardVisible([[maybe_unused]] bool visible) {
   // not supported
}

void InputHandlerImpl::initIOHID() {
	static auto is_init = false;
	if(is_init) return;

	auto tis = TISCopyCurrentKeyboardLayoutInputSource();
	m_layout_data = static_cast<CFDataRef>(TISGetInputSourceProperty(tis, kTISPropertyUnicodeKeyLayoutData));

	CFRetain(m_layout_data);
	m_layout = const_cast<UCKeyboardLayout*>(reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(m_layout_data)));

	CFRelease(tis);

	m_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

	IOHIDManagerOpen(m_manager, kIOHIDOptionsTypeNone);

	initializeKeyboard();

	is_init = true;
}

void InputHandlerImpl::initializeKeyboard() {
	auto keyboards = copyDevices(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);

	auto keyboardCount = CFSetGetCount(keyboards);

	auto devices_array = std::vector<CFTypeRef>{};
	devices_array.resize(keyboardCount, nullptr);

	CFSetGetValues(keyboards, std::data(devices_array));

	for (auto i = CFIndex{0}; i < keyboardCount; ++i) {
		IOHIDDeviceRef keyboard = reinterpret_cast<IOHIDDeviceRef>(const_cast<void*>(devices_array[i]));
		loadKeyboard(keyboard);
	}

	CFRelease(keyboards);
}


void InputHandlerImpl::loadKeyboard(IOHIDDeviceRef keyboard) {
	auto keys = IOHIDDeviceCopyMatchingElements(keyboard,
													  nullptr,
													  kIOHIDOptionsTypeNone);

	auto keys_count = CFArrayGetCount(keys);

	for (auto i = CFIndex{0}; i < keys_count; ++i) {
		IOHIDElementRef key = reinterpret_cast<IOHIDElementRef>(const_cast<void*>(CFArrayGetValueAtIndex(keys, i)));

		if (IOHIDElementGetUsagePage(key) != kHIDPage_KeyboardOrKeypad)
			continue;

		loadKey(key);
	}

	// Release unused stuff
	CFRelease(keys);
}

void InputHandlerImpl::loadKey(IOHIDElementRef key) {
	auto usage_code   = IOHIDElementGetUsage(key);
	auto virtual_code = usageToVirtualCode(usage_code);

	if (virtual_code == 0xff)
		return;

	auto dead_key_state     = core::UInt32{0};
	const auto max_string_length  = UniCharCount{4};
	auto actual_string_length = UniCharCount{0};

	auto unicode_string = std::vector<UniChar>{};
	unicode_string.resize(max_string_length);

	UCKeyTranslate(m_layout,
				   virtual_code,
				   kUCKeyActionDown,
				   0x100,
				   LMGetKbdType(),
				   kUCKeyTranslateNoDeadKeysBit,
				   &dead_key_state,
				   max_string_length,
				   &actual_string_length,
				   std::data(unicode_string));

	auto code = Key::UNKNOW;

	if (actual_string_length > 0)
		code = localizedKeyToStormKey(unicode_string[0]);

	if (code == Key::UNKNOW)
		code = nonLocalizedKeytoStormKey(virtual_code);

	if (code != Key::UNKNOW)
	{
		m_keys[static_cast<core::ArraySize>(code)].emplace_back(key);

		CFRetain(m_keys[static_cast<core::ArraySize>(code)].back());
	}
}

CFDictionaryRef InputHandlerImpl::copyDevicesMask(core::UInt32 page, core::UInt32 usage) {
	auto dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 2,
															&kCFTypeDictionaryKeyCallBacks,
															&kCFTypeDictionaryValueCallBacks);

	auto value = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
	CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsagePageKey), value);
	CFRelease(value);

	value = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
	CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsageKey), value);
	CFRelease(value);

	return dict;
}

CFSetRef InputHandlerImpl::copyDevices(core::UInt32 page, core::UInt32 usage) {
	auto mask = copyDevicesMask(page, usage);

	IOHIDManagerSetDeviceMatching(m_manager, mask);

	CFRelease(mask);
	mask = nullptr;

	auto devices = IOHIDManagerCopyDevices(m_manager);
	if (devices == nullptr)
		return nullptr;

	CFIndex deviceCount = CFSetGetCount(devices);
	if (deviceCount < 1)
	{
		CFRelease(devices);
		return nullptr;
	}

	return devices;
}
