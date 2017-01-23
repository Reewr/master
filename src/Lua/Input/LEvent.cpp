#include "LEvent.hpp"
#include "../../Input/Event.hpp"

#include <sol.hpp>

void Lua::event_as_lua(sol::state& state) {
  // Add the enum values that the Event class uses

  // clang-format off
  state["InputEventType"] = state.create_table_with(
      "Consumed"      , Input::Event::Type::Consumed,
      "MouseMovement" , Input::Event::Type::MouseMovement,
      "MousePress"    , Input::Event::Type::MousePress,
      "MouseRelease"  , Input::Event::Type::MouseRelease,
      "MouseScroll"   , Input::Event::Type::MouseScroll,
      "KeyPress"      , Input::Event::Type::KeyPress,
      "KeyRelease"    , Input::Event::Type::KeyRelease,
      "CharacterInput", Input::Event::Type::CharacterInput);
  // clang-format on

  // Add all the functions that the Event class exposes.
  // The idea is that you're never allowed to do `Event.new()`, since
  // that is only done by the Engine.
  // clang-format off
  state.new_usertype<Input::Event>("InputEvent",
    "new"            , sol::no_constructor,
    "type"           , &Input::Event::type,
    "hasAlt"         , &Input::Event::hasAlt,
    "hasCtrl"        , &Input::Event::hasCtrl,
    "hasSuper"       , &Input::Event::hasSuper,
    "hasShift"       , &Input::Event::hasShift,
    "key"            , &Input::Event::key,
    "button"         , &Input::Event::button,
    "isAction"       , &Input::Event::isAction,
    "isKeyHeldDown"  , &Input::Event::isKeyHeldDown,
    "keyPressed"     , &Input::Event::keyPressed,
    "buttonPressed"  , &Input::Event::buttonPressed,
    "position"       , &Input::Event::position,
    "scrollLeft"     , &Input::Event::scrollLeft,
    "scrollRight"    , &Input::Event::scrollRight,
    "scrollUp"       , &Input::Event::scrollUp,
    "scrollDown"     , &Input::Event::scrollDown,
    "hasBeenHandled" , &Input::Event::hasBeenHandled,
    "character"      , &Input::Event::character,
    "prevType"       , &Input::Event::prevType,
    "state"          , &Input::Event::state,
    "sendStateChange", &Input::Event::sendStateChange,
    "stopPropgation" , &Input::Event::stopPropgation);
  // clang-format on
}
