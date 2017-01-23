#pragma once

namespace sol {
  class state;
}

namespace Lua {
  void state_as_lua(sol::state& state);
}
