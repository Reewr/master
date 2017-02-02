#include "LCFG.hpp"
#include "../../Utils/CFG.hpp"

#include <sol.hpp>

void Lua::cfg_as_lua(sol::state& state) {
  // Add Graphics as a lua struct, so it can
  // be accessed when using CFG.
  //
  // Scoping is done to make this section slightly
  // bit cleaner
  //
  // CFG::Graphics
  // clang-format off
  {
    // Add the type of constructors, since we don't have one
    // we just say no types.
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Graphics> type(ctor,
      "aliasing"     , &CFG::Graphics::aliasing,
      "aniso"        , &CFG::Graphics::aniso,
      "aspect"       , &CFG::Graphics::aspect,
      "monitor"      , &CFG::Graphics::monitor,
      "res"          , &CFG::Graphics::res,
      "shadowSamples", &CFG::Graphics::shadowSamples,
      "shadowRes"    , &CFG::Graphics::shadowRes,
      "vsync"        , &CFG::Graphics::vsync,
      "viewDistance" , &CFG::Graphics::viewDistance,
      "winMode"      , &CFG::Graphics::winMode);

    state.set_usertype("_CFG_Graphics", type);
  }

  // CFG::General
  {
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::General> type(ctor,
      "debug", &CFG::General::debug);

    state.set_usertype("_CFG_General", type);
  }

  // CFG::Camera
  {
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Camera> type(ctor,
      "rotSpeed" , &CFG::Camera::rotSpeed,
      "rotInvH"  , &CFG::Camera::rotInvH,
      "rotInvV"  , &CFG::Camera::rotInvV,
      "zoomSpeed", &CFG::Camera::zoomSpeed,
      "zoomInv"  , &CFG::Camera::zoomInv);

    state.set_usertype("_CFG_Camera", type);
  }

  // CFG::Console
  {
    sol::constructors<sol::types<>> ctor;

    sol::usertype<CFG::Console> type(ctor,
      "enabled", &CFG::Console::enabled);

    state.set_usertype("_CFG_Console", type);
  }

  // ActB
  {
    sol::constructors<sol::types<>, sol::types<int>, sol::types<int, int>> ctor;

    sol::usertype<ActB> type(ctor,
        "key1", &ActB::key1,
        "key2", &ActB::key2);

    state.set_usertype("_CFG_ActB", type);
  }

  // CFG::Bindings
  {

    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Bindings> type(ctor,
      "moveUp"     , &CFG::Bindings::moveUp,
      "moveDown"   , &CFG::Bindings::moveDown,
      "moveLeft"   , &CFG::Bindings::moveLeft,
      "moveRight"  , &CFG::Bindings::moveRight,
      "pauseMenu"  , &CFG::Bindings::pauseMenu,
      "rotate"     , &CFG::Bindings::rotate,
      "showConsole", &CFG::Bindings::showConsole,
      "screenshot" , &CFG::Bindings::screenshot);

    state.set_usertype("_CFG_Bindings", type);
  }

  // CFG::Audio
  {
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Audio> type(ctor,
      "SFXVolume"   , &CFG::Audio::SFXVolume,
      "musicVolume" , &CFG::Audio::musicVolume,
      "masterVolume", &CFG::Audio::masterVolume);

    state.set_usertype("_CFG_Audio", type);
  }

  // Finally, since all the child structures of the CFG has been
  // defined, we can now define the CFG itself.
  sol::constructors<sol::types<>> ctor;

  // add the variables that should be accessible via lua.
  sol::usertype<CFG> type(ctor,
    "graphics", &CFG::graphics,
    "general" , &CFG::general,
    "camera"  , &CFG::camera,
    "console" , &CFG::console,
    "bindings", &CFG::bindings,
    "audio"   , &CFG::audio);

  state.set_usertype("CFG", type);
  // clang-format on

  // At this point, you can load the state and use the CFG
  // as such:
  //
  // myCFG = CFG.new()
  // print(myCFG.graphics.res.x)
  // print(myCFG.bindings.moveUp.key1)
  // -- and so on.
}
