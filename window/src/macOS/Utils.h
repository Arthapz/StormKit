#pragma once

#include <storm/core/Types.hpp>

#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>
#include <storm/window/VideoSettings.hpp>

#import <Foundation/NSGeometry.h>

storm::window::MouseButton toStormMouseButton(int button);
storm::window::Key localizedKeyToStormKey(char code);
storm::window::Key nonLocalizedKeytoStormKey(unsigned short code);
storm::core::Vector2u toStormVec(NSPoint point);
storm::core::Extentu toStormVec(CGSize size);
NSPoint fromStormVec(const storm::core::Vector2u &point);
storm::core::UInt8 usageToVirtualCode(storm::core::UInt32 usage);
