local StateType = require 'State.Type'
local state = {}

--[[
  When called, tells the engine that it should
  go back the previous state. If the current
  state is the last state, it will exit.

  This all depends on how the State stack is built.

  This function is only available if `cfg.console.enabled` is true

  @class function
]]
function state.prev()
  console:log("Returning to previous state")
  currentEvent:sendStateChange(StateType.Quit)
  currentEvent:stopPropgation()
end

--[[
  When called, tells the engine that it should
  go reload the current state. This means that
  it will unload everything in the current state before
  creating it a new again.

  This function is only available if `cfg.console.enabled` is true

  @class function
]]
function state.reload()
  console:log("Reloading current state")
  currentEvent:sendStateChange(StateType.Refresh)
  currentEvent:stopPropgation()
end

--[[
  When called, tells the engine that it should
  do a full reload of the entire Window, GL context and
  current state.

  This means that the engine will basically be recreated.

  This function is only available if `cfg.console.enabled` is true

  @class function
]]
function state.windowRefresh()
  console:log("Reloading window")
  currentEvent:sendStateChange(StateType.WinRefresh)
  currentEvent:stopPropgation()
end

function state.load(statetoLoad)
  console:log("Loading State")
  currentEvent:sendStateChange(statetoLoad)
  currentEvent:stopPropgation()
end

--[[

  Reloads the Lua Engine by sending a state change
  to the Engine.

  If variables are defined by using the console and not
  stored in a file, they will be erased.

  This function is only available if `cfg.console.enabled` is true

  @class function
]]
function state.luaReload()
  console:log("Reloading LUA engine and files")
  currentEvent:sendStateChange(StateType.LuaReload)
  currentEvent:stopPropgation()
end

return state
