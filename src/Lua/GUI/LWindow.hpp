#pragma once

namespace sol {
  class state;
}

namespace Lua {

  // Exports the GUI Text class exactly as it is
  void window_as_lua(sol::state& state);
}
