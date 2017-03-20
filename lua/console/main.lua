--[[
  This file contains everything that depends on
  the console in order to work.

  A lot of the functions within this file, and the files
  that this requires, is utility and debug functions that
  doesnt add anything to do the game, but helps with developing.

  The module exposes a couple of global variables:

  - `state`
  - `hud`

  The `state` object contains functions that helps with
  interacting with states through events.

  The `hud` object contains various hud element objects.
  For instance, it contains `fps`, which has `hide` and `show`
  functions to hide/show the FPS on the screen.

  Lastly, it overrides the normal `print` function with one
  that prints to both the Engine's stdout and to the console.

  This module is only loaded if `cfg.console.enabled` is true

  @class module
]]
local StateType = require 'State.Type'
-- These are the global values that the console
-- exposes
_print = print
state  = require 'console/state'
hud    = {
  fps = require 'debug/fps'
}

-- Override print so that it prints
-- to console and therefore to stdout
function print(...)

  -- No printing of debug is off
  if not cfg.general.debug then
    return
  end

  local s = ''

  for i, v in ipairs({...}) do
    s = s .. tostring(v) .. ' '
  end

  if console == nil then
    _print(s)
  else
    console:log('\\<102,217,239,255:>LUA:\\</> ' .. s)
  end
end

--[[

  Exists the entire game by sending a state change
  to the Engine. Will stop everything and close the engine

  This function is only available if `cfg.console.enabled` is true.

  @class function
]]
function exit()
  console:log("Quitting")
  currentEvent:sendStateChange(StateType.QuitAll)
  currentEvent:stopPropgation()
end

--[[

  As with `exit()`, thus function just exits the game.
  It is also only available if `cfg.console.enabled` is true.

  @see exit
  @class function
]]
function quit()
  exit()
end

hud.fps.show()
