local state = {}

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

return state
