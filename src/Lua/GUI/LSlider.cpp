#include "LSlider.hpp"

#include "../../GUI/Slider.hpp"
#include "../../Input/Event.hpp"

#include <sol.hpp>

#include "../Math/LMath.hpp"

void Lua::slider_as_lua(sol::state& state) {
  auto guiTable = state["GUI"];

  if (!guiTable.valid())
    state.create_named_table("GUI");

  sol::table GUITable = state["GUI"];

  sol::constructors<
    sol::types<const mmm::vec2&>,
    sol::types<const mmm::vec2&, float>,
    sol::types<const mmm::vec2&, float, const std::string&>> dropdownCtor;

  // clang-format off
  sol::usertype<Slider> dropdownType(dropdownCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Slider::*)() const) &Slider::isVisible,
                                 (void(Slider::*)(bool)) &Slider::isVisible),

    "isClickable", sol::overload((bool(Slider::*)() const) &Slider::isClickable,
                                 (void(Slider::*)(bool)) &Slider::isClickable),

    "isAnimating", sol::overload((bool(Slider::*)() const) &Slider::isAnimating,
                                 (void(Slider::*)(bool)) &Slider::isAnimating),

    "isMinimized", sol::overload((bool(Slider::*)() const) &Slider::isMinimized,
                                 (void(Slider::*)(bool)) &Slider::isMinimized),

    "isMouseOver", sol::overload((bool(Slider::*)() const) &Slider::isMouseOver,
                                 (void(Slider::*)(bool)) &Slider::isMouseOver),

    "hasChanged" , sol::overload((bool(Slider::*)() const) &Slider::hasChanged,
                                 (void(Slider::*)(bool)) &Slider::hasChanged),

    "update"               , &Slider::update,
    "isInside"             , &Slider::isInside,
    "input"                , &Slider::input,
    "setInputHandler"      , &Slider::setInputHandler,
    "defaultInputHandler"  , &Slider::defaultInputHandler,
    "box"                  , &Slider::box,
    "position"             , &Slider::position,
    "size"                 , &Slider::size,
    "offset"               , &Slider::offset,
    "setSize"              , &Slider::setSize,

    // Functions specific to Slider
    "value"     , &Slider::value,
    "moveSlider", &Slider::moveSlider,
    "setSlider" , &Slider::setSlider,
    "draw"      , &Slider::draw,
    sol::base_classes, sol::bases<GUI>());

  GUITable.set_usertype("Slider", dropdownType);
  // clang-format on
}
