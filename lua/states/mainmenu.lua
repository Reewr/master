-- Engine requires
local vec2            = require 'Math.vec2'
local Window          = require 'GUI.Window'
local TextColor       = require 'GUI.Text.Color'
local MenuOrientation = require 'GUI.Menu.Orientation'
local MenuSettings    = require 'GUI.Menu.Settings'
local Rect            = require 'Shape.Rectangle'
local Util            = require 'Util'
local Keys            = require 'Input.Keys'
local States          = require 'State.Type'

-- File dependencies
local OptionsMenu     = require 'gui/optionsmenu'

local mainMenu = Window.new("NONE", Rect.new(cfg.graphics.res.x - 375,
                                             cfg.graphics.res.y - 175,
                                             cfg.graphics.res.x,
                                             cfg.graphics.res.y))

local menuItems = {"Master Thesis", "Start Game", "Options", "Exit"}
mainMenu:isVisible(true)
local ms = MenuSettings.new(15.0, 50.0, MenuOrientation.Vertical)

mainMenu:addMenu("MainMenu",
                 Util.toVectorStr(menuItems),
                 vec2.new(),
                 ms)


local menu = mainMenu:menu("MainMenu")

local onEvent = function(event)
  if not mainMenu:isVisible() or mainMenu:isAnimating() then
    return
  end

  mainMenu:defaultInputHandler(event)

  if event:hasBeenHandled() then
    return
  end

  local hasConfirm = event:keyPressed(Keys.Enter) or
                     event:buttonPressed(Keys.MouseButton1)

  if not hasConfirm then
    return
  end

  local menuChoose = menu:getActiveMenu()

  if menuChoose == 0 then
    event:sendStateChange(States.MasterThesis)
  elseif menuChoose == 1 then
    event:sendStateChange(States.Game)
  elseif menuChoose == 2 then
  elseif menuChoose == 3 then
    event:sendStateChange(States.Quit)
  end

  event:stopPropgation()
end

mainMenu:setInputHandler(onEvent)

-- addtoDrawable(mainMenu)
