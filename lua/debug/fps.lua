-- This module defines a new object on the global debug object called
-- `fps`. The `fps` object has two functions:
--
-- - `show(x, y, textSize)`
-- - `hide()`
--
-- The `x` and `y` are coordinates for where to draw the fps, but this
-- defaults to upper right corner of the screen. The `textSize` defaults to 16
--
-- It returns the FPS object that can be assigned to where ever you want it.
local FPS = {}
local textElement = nil

-- Shows the FPS at position (X, Y) with size `textSize`
-- which defaults to (screenRes.x - 100, 100, 16)
--
-- Calling this multiple times after each other has no purpose
-- as it will just hide and delete the previous one before
-- creating a new text element
function FPS.show(x, y, textSize)
  if textElement ~= nil then
    FPS:hide()
  end

  print("Showing FPS")

  -- set default X and Y if they are not numbers
  x = type(x) == "number" and x or cfg.graphics.res.x - 50
  y = type(y) == "number" and y or 25
  textSize = type(x) == "number" and texSize or 16

  textElement = GUI.Text.new("Font::Dejavu",
                             "0",
                             vec2.new(x, y),
                             textSize,
                             GUI.TextColor.Yellow)
  local element   = {}
  local frames    = 0
  local totalTime = 0

  -- just a wrapper around draw for the text so that
  -- the update function can also be wrapped.
  function element:draw()
    textElement:draw()
  end

  -- on update, update the number of frames only
  -- showing them when a second has passed.
  function element:update(deltaTime)
    totalTime = totalTime + deltaTime
    frames = frames + 1

    if totalTime < 1 then
      return
    end

    strFrames = tostring(frames)
    stored    = textElement:getText()
    totalTime = 0
    frames    = 0

    if strFrames ~= stored then
      textElement:setText(strFrames)
    end
  end

  addtoDrawable(element, 'fpsText')
end

-- Hides the text if it is showing by disabling
-- it entirely and removing the text object
function FPS.hide()
  if textElement == nil then
    return
  end

  print("Hiding FPS")
  removefromDrawable('fpsText')
  textElement = nil
end

return FPS
