-- This file is responsible for having the handlers for the console
-- enabled.
state = {}

function exit()
  console:log("Quitting")
  currentEvent:sendStateChange(States.QuitAll)
  currentEvent:stopPropgation()
end

function quit()
  exit()
end

function state.prev()
  console:log("Returning to previous state")
  currentEvent:sendStateChange(States.Quit)
  currentEvent:stopPropgation()
end

function state.reload()
  console:log("Reloading current state")
  currentEvent:sendStateChange(States.Refresh)
  currentEvent:stopPropgation()
end

function state.windowRefresh()
  console:log("Reloading window")
  currentEvent:sendStateChange(States.WinRefresh)
  currentEvent:stopPropgation()
end

function lua.reload()
  console:log("Reloading LUA engine and files")
  currentEvent:sendStateChange(States.LuaReload)
  currentEvent:stopPropgation()
end
