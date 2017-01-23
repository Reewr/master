#pragma once

namespace sol {
  class state;
}

namespace Lua {
  void engine_as_lua(sol::state& state);
}
