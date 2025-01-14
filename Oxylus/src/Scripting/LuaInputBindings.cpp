﻿#include "LuaInputBindings.h"

#include <sol/state.hpp>
#include <sol/string_view.hpp>

#include "Core/Input.h"
#include "Core/Keycodes.h"

namespace Oxylus::LuaBindings {
void bind_lua_input(const Ref<sol::state>& state) {
  auto input = (*state)["Input"].get_or_create<sol::table>();

  input.set_function("get_key_pressed", [](const KeyCode key) -> bool { return Input::get_key_pressed(key); });
  input.set_function("get_key_held", [](const KeyCode key) -> bool { return Input::get_key_held(key); });

  input.set_function("get_mouse_clicked", [](const MouseCode key) -> bool { return Input::get_mouse_clicked(key); });
  input.set_function("get_mouse_held", [](const MouseCode key) -> bool { return Input::get_mouse_held(key); });
  input.set_function("get_mouse_position", []() -> Vec2 { return Input::get_mouse_position(); });
  input.set_function("get_scroll_offset", []() -> float { return Input::get_mouse_scroll_offset_y(); });

  // TODO: controller support
  //input.set_function("get_controller_axis", [](int id, int axis) -> float { return Input::get_controller_axis(id, axis); });
  //input.set_function("get_controller_name", [](int id) -> std::string { return Input::get_controller_name(id); });
  //input.set_function("get_controller_hat", [](int id, int hat) -> int { return Input::get_controller_hat(id, hat); });
  //input.set_function("is_controller_button_pressed", [](int id, int button) -> bool { return Input::is_controller_button_pressed(id, button); });

  const std::initializer_list<std::pair<sol::string_view, KeyCode>> key_items = {
    {"A", KeyCode::A},
    {"B", KeyCode::B},
    {"C", KeyCode::C},
    {"D", KeyCode::D},
    {"E", KeyCode::E},
    {"F", KeyCode::F},
    {"H", KeyCode::G},
    {"G", KeyCode::H},
    {"I", KeyCode::I},
    {"J", KeyCode::J},
    {"K", KeyCode::K},
    {"L", KeyCode::L},
    {"M", KeyCode::M},
    {"N", KeyCode::N},
    {"O", KeyCode::O},
    {"P", KeyCode::P},
    {"Q", KeyCode::Q},
    {"R", KeyCode::R},
    {"S", KeyCode::S},
    {"T", KeyCode::T},
    {"U", KeyCode::U},
    {"V", KeyCode::V},
    {"W", KeyCode::W},
    {"X", KeyCode::X},
    {"Y", KeyCode::Y},
    {"Z", KeyCode::Z},
    //{ "UNKOWN", KeyCode::Unknown },
    {"Space", KeyCode::Space},
    {"Escape", KeyCode::Escape},
    {"APOSTROPHE", KeyCode::Apostrophe},
    {"Comma", KeyCode::Comma},
    {"MINUS", KeyCode::Minus},
    {"PERIOD", KeyCode::Period},
    {"SLASH", KeyCode::Slash},
    {"SEMICOLON", KeyCode::Semicolon},
    {"EQUAL", KeyCode::Equal},
    {"LEFT_BRACKET", KeyCode::LeftBracket},
    {"BACKSLASH", KeyCode::Backslash},
    {"RIGHT_BRACKET", KeyCode::RightBracket},
    //{ "BACK_TICK", KeyCode::BackTick },
    {"Enter", KeyCode::Enter},
    {"Tab", KeyCode::Tab},
    {"Backspace", KeyCode::Backspace},
    {"Insert", KeyCode::Insert},
    {"Delete", KeyCode::Delete},
    {"Right", KeyCode::Right},
    {"Left", KeyCode::Left},
    {"Down", KeyCode::Down},
    {"Up", KeyCode::Up},
    {"PageUp", KeyCode::PageUp},
    {"PageDown", KeyCode::PageDown},
    {"Home", KeyCode::Home},
    {"End", KeyCode::End},
    {"CapsLock", KeyCode::CapsLock},
    {"ScrollLock", KeyCode::ScrollLock},
    {"NumLock", KeyCode::NumLock},
    {"PrintScreen", KeyCode::PrintScreen},
    {"Pasue", KeyCode::Pause},
    {"LeftShift", KeyCode::LeftShift},
    {"LeftControl", KeyCode::LeftControl},
    {"LeftAlt", KeyCode::LeftAlt},
    {"LeftSuper", KeyCode::LeftSuper},
    {"RightShift", KeyCode::RightShift},
    {"RightControl", KeyCode::RightControl},
    {"RightAlt", KeyCode::RightAlt},
    {"RightSuper", KeyCode::RightSuper},
    {"Menu", KeyCode::Menu},
    {"F1", KeyCode::F1},
    {"F2", KeyCode::F2},
    {"F3", KeyCode::F3},
    {"F4", KeyCode::F4},
    {"F5", KeyCode::F5},
    {"F6", KeyCode::F6},
    {"F7", KeyCode::F7},
    {"F8", KeyCode::F8},
    {"F9", KeyCode::F9},
    {"F10", KeyCode::F10},
    {"F11", KeyCode::F11},
    {"F12", KeyCode::F12},
    {"KeyCodepad0", KeyCode::D0},
    {"KeyCodepad1", KeyCode::D1},
    {"KeyCodepad2", KeyCode::D2},
    {"KeyCodepad3", KeyCode::D3},
    {"KeyCodepad4", KeyCode::D4},
    {"KeyCodepad5", KeyCode::D5},
    {"KeyCodepad6", KeyCode::D6},
    {"KeyCodepad7", KeyCode::D7},
    {"KeyCodepad8", KeyCode::D8},
    {"KeyCodepad9", KeyCode::D9},
    {"Decimal", KeyCode::Period},
    {"Divide", KeyCode::Slash},
    {"Multiply", KeyCode::KPMultiply},
    {"Subtract", KeyCode::Minus},
    {"Add", KeyCode::KPAdd},
    {"KPEqual", KeyCode::KPEqual}
  };
  state->new_enum<KeyCode, true>("Key", key_items);

  const std::initializer_list<std::pair<sol::string_view, MouseCode>> mouse_items = {
    {"Left", MouseCode::ButtonLeft},
    {"Right", MouseCode::ButtonRight},
    {"Middle", MouseCode::ButtonMiddle},
  };
  state->new_enum<MouseCode, true>("MouseButton", mouse_items);
}
}
