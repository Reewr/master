#include "State.hpp"
#include "../../State/State.hpp"

#include <sol.hpp>

void Lua::state_as_lua(sol::state& state) {
  // clang-format off
  state["States"] = state.create_table_with(
    "QuitAll"         , States::QuitAll,
    "Quit"            , States::Quit,
    "Init"            , States::Init,
    "OptionsMenuClose", States::OptionsMenuClose,
    "OptionsMenuOpen" , States::OptionsMenuOpen,
    "PauseMenuOpen"   , States::PauseMenuOpen,
    "PauseMenuClose"  , States::PauseMenuClose,
    "MainMenu"        , States::MainMenu,
    "Game"            , States::Game,
    "MasterThesis"    , States::MasterThesis,
    "Refresh"         , States::Refresh,
    "WinRefresh"      , States::WinRefresh,
    "NoChange"        , States::NoChange,
    "LuaReload"       , States::LuaReload);
  // clang-format on
}
