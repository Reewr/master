--[[

  This module defines a new object on the global debug object called
  `fps`. The `fps` object has two functions:

  - `show(x, y, textSize)`
  - `hide()`

  The `x` and `y` are coordinates for where to draw the fps, but this
  defaults to upper right corner of the screen. The `textSize` defaults to 16

  It returns the FPS object that can be assigned to where ever you want it.

]]
local Text = require 'GUI.Text'
local vec2 = require 'Math.vec2'
local FPS = {}
local textElement = nil

--[[

  Shows the FPS at position (X, Y) with size `textSize`
  which defaults to (screenRes.x - 100, 100, 16)

  Calling this multiple times after each other has no purpose
  as it will just hide and delete the previous one before
  creating a new text element

  @class function
  @param x         x-coordinate
  @param y         y-coordinate
  @param textSize  the size of the text
  @param printInfo whether or not to print FPS as it updates

]]
function FPS.show(x, y, textSize, printInfo)
  if textElement ~= nil then
    FPS:hide()
  end

  local _x         = type(x) == "number" and x or cfg.graphics.res.x - 50
  local _y         = type(y) == "number" and y or 25
  local _textSize  = type(textSize) == "number" and textSize or 16
  local _printInfo = false

  -- Check if printInfo was sent as any other variable
  if type(x) == "boolean" then _printInfo = x end
  if type(y) == "boolean" then _printInfo = y end
  if type(textSize) == "boolean" then _printInfo = textSize end
  if type(printInfo) == "boolean" then _printInfo = printInfo end

  print("Showing FPS")

  textElement = Text.new("Font::Dejavu",
                         "0",
                         vec2.new(_x, _y),
                         _textSize,
                         Text.Color.Yellow)
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

    local strFrames = tostring(frames / totalTime)
    local stored    = textElement:getText()
    totalTime = 0
    frames    = 0

    if _printInfo then
      print("FPS: ", strFrames)
    end

    if strFrames ~= stored then
      textElement:setText(strFrames)
    end
  end

  addtoDrawable(element, 'fpsText')
end

--[[
  Hides the text if it is showing by disabling
  it entirely and removing the text object

  @class function
]]
function FPS.hide()
  if textElement == nil then
    return
  end

  print("Hiding FPS")
  removefromDrawable('fpsText')
  textElement = nil
end

return FPS
