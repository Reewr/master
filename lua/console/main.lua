-- This is the main file of the console. Usually
-- this is the only file you need to require.

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
  s = ''

  for i, v in ipairs({...}) do
    s = s .. tostring(v) .. ' '
  end

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

-- Reloads the Lua Engine
function lua.reload()
  console:log("Reloading LUA engine and files")
  currentEvent:sendStateChange(States.LuaReload)
  currentEvent:stopPropgation()
end
