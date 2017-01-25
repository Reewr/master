-- This file is responsible for having the handlers for the console
-- enabled.
state = {}

-- Override print so that it prints
-- to console and therefore to stdout
_print = print

function print(...)
  s = tostring(...)
  console:log('\\<102,217,239,255:>LUA:\\</> ' .. s)
end

-- Exists the entire game.
function exit()
  console:log("Quitting")
  currentEvent:sendStateChange(States.QuitAll)
  currentEvent:stopPropgation()
end

-- Synonomous with quit
function quit()
  exit()
end

-- Goes back a state. If the state is the current
-- state in the last, it will quit.
function state.prev()
  console:log("Returning to previous state")
  currentEvent:sendStateChange(States.Quit)
  currentEvent:stopPropgation()
end

-- Reloads the current state
function state.reload()
  console:log("Reloading current state")
  currentEvent:sendStateChange(States.Refresh)
  currentEvent:stopPropgation()
end

-- Reloads the current state and reloads the GL context
function state.windowRefresh()
  console:log("Reloading window")
  currentEvent:sendStateChange(States.WinRefresh)
  currentEvent:stopPropgation()
end

-- Reloads the Lua Engine
function lua.reload()
  console:log("Reloading LUA engine and files")
  currentEvent:sendStateChange(States.LuaReload)
  currentEvent:stopPropgation()
end
