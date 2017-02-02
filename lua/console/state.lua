local state = {}

-- Goes back a state. If the state is the current
-- state in the last, it will quit.

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
  currentEvent:sendStateChange(States.Quit)
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
  currentEvent:sendStateChange(States.Refresh)
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
  currentEvent:sendStateChange(States.WinRefresh)
  currentEvent:stopPropgation()
end

return state
