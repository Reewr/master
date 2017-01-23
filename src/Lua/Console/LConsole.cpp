#include "LConsole.hpp"
#include "../../Console/Console.hpp"

#include <sol.hpp>

void Lua::console_as_lua(sol::state& state) {
  // clang-format off
  state.new_usertype<Console>("GameConsole",
    "new"    , sol::no_constructor,
    "log"    , &Console::log,
    "error"  , &Console::error,
    "warn"   , &Console::warn);
  // clang-format on
}
