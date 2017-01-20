-- This file is responsible for having the handlers for the console
-- enabled.

function exit()
  console:log("Quitting")
  currentEvent:sendStateChange(States.QuitAll)
  currentEvent:stopPropgation()
end

function quit()
  exit()
end

function prevState()
  console:log("Returning to previous state")
  currentEvent:sendStateChange(States.Quit)
  currentEvent:stopPropgation()
end
