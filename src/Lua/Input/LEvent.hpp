#pragma once

namespace sol {
  class state;
}

namespace Lua {
  // This function adds the Event class as an object in Lua. The global
  // objects added using this is:
  //
  // - InputEventType
  // - InputEvent
  //
  // InputEventType is a table with enums representing the type of event
  //
  // InputEvent is exposing the Event class fully
  void event_as_lua(sol::state& state);
}
