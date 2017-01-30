-- This file describes the implementation and setup of the options menu.
local OptionsMenu = {}

local startX = cfg.graphics.res.x * 0.50 - 500
local startY = cfg.graphics.res.y * 0.50 - 350

local rect         = Rectangle.new(startX, startY, 1000, 700)
local window       = GUI.Window.new("Texture::Background", rect)
local categories   = {"Audio", "Game", "Graphics", "Keybindings", "Mouse"}
local activeWindow = ""

-- Add the sidebar menu
window:addMenu("Category",
               toVectorStr(categories),
               vec2.new(25, 100),
               GUI.MenuSettings.new(20,
                                    50,
                                    GUI.Orientation.Vertical,
                                    GUI.TextColor.White))

local category  = window:menu("Category")
local innerRect = Rectangle.new(200, 54, rect.size.x - 200, rect.size.y - 125)

-- -- Accept and cancel buttons
category:addMenuItem("Accept", Rectangle.new(rect.middle().x - 200,
                                             rect.bottomright().y - 50))

category:addMenuItem("Cancel", Rectangle.new(rect.middle().x + 100,
                                             rect.bottomright().y - 50))

-- window:addWindow("Audio", innerRect)
-- window:addWindow("Graphics", innerRect)
-- window:addWindow("Keybindings", innerRect)
-- window:addWindow("Mouse", innerRect)
-- window:addWindow("Game", innerRect)

-- local windows = {
--   Audio       = window:window("Audio"),
--   Graphics    = window:window("Graphics"),
--   Keybindings = window:window("Keybindings"),
--   Mouse       = window:window("Mouse"),
--   Game        = window:window("Game")
-- }

-- -- This function basically clears everything. In order to keep everything away
-- -- from everything else, I've created this function
-- function addElements()
--   local windowOptions = {
--     Graphics = {
--       menu = {
--         name = "Graphics",
--         position = vec2.new(55, 35),
--         menuOptions = GUI.Menu.MenuSettings(15, 1, 0, 1),
--         items = {{
--           pos = vec2.new(55, 35),
--           text = "Resolution"
--         }, {
--           pos = vec2.new(225, 35),
--           text = "Vsync"
--         }, {
--           pos = vec2.new(400, 35),
--           text = "Display Mode"
--         }, {
--           pos = vec2.new(575, 35),
--           text = "View distance"
--         }, {
--           pos = vec2.new(90, 125),
--           text = "Monitor"
--         }, {
--           pos = vec2.new(280, 125),
--           text = "Anti-aliasing"
--         }, {
--           pos = vec2.new(516, 125),
--           text = "Anisotrophic filtering"
--         }, {
--           pos = vec2.new(50, 225),
--           text = "Shadow Sampling"
--         }, {
--           pos = vec2.new(300, 225),
--           text = "Shadow Resolution"
--         }}
--       },
--       elements = {
--       }
--     },

--     Audio = {
--       menu = {
--         name = "Audio",
--         position = vec2.new(225, 35),
--         menuOptions = GUI.Menu.MenuSettings(15, 1, 180, 1),
--         items = {{
--           pos = vec2.new(55, 35),
--           text = "Master Volume"
--         }, {
--           pos = vec2.new(225, 35),
--           text = "SFX Volume"
--         }, {
--           pos = vec2.new(400, 35),
--           text = "Music Volume"
--         }}
--       }
--     },

--     Game = {
--       menu = {
--         name = "Game",
--         position = vec2.new(50, 50),
--         menuOptions = GUI.Menu.MenuSettings(15, 1, 150, 1),
--         items = {{
--           pos = vec2.new(50, 50),
--           text = "Camera Rotation"
--         }, {
--           pos = vec2.new(80, 130),
--           text = "Inverse Horizontal"
--         }, {
--           pos = vec2.new(430, 50),
--           text = "Camera Zoom"
--         }, {
--           pos = vec2.new(460, 50),
--           text = "Camera Inverse"
--         }}
--       }
--     },

--     Keybindings = {
--       menu = {
--         name = "Keybindings",
--         position = vec2.new(320, 60),
--         menuOptions = GUI.Menu.MenuSettings(15, 1, 150, 1),
--         items = {{
--           pos = vec2.new(320, 60),
--           text = "Key 1"
--         }, {
--           pos = vec2.new(495, 60),
--           text = "Key 2"
--         }}
--       }
--     }
--   }

--   for key, value in windowOptions do
--     windows[key]:addMenu(value.name, value.position, value.menuOptions)
--     menu = windows:menu(value.name)
--     for item in value.items do
--       menu:addMenuItem(item.pos, item.text)
--     end
--   end

--   -- windows.Graphics.addDropdown("res",
--   --                              Glfw.getAvailableResolutionsAsStrings(),
--   --                              vec2.new(50, 60))
--   -- windows.Graphics.addDropdown("vsync", {"On", "Off"}, vec2.new(230, 60))
--   -- windows.Graphics.addDropdown("")
-- end

-- -- addElements()

-- -- This function handles clicks or presses enter while in the options menu
-- -- It will only be invoked if the mouse click / key press has not already
-- -- been handled
-- function onClick(event)
--   local item                  = category:getActiveMenu()
--   local hasMenuActiveMenuItem = item >= 0 and item < 5

--   -- hide the currently active window if another item is active
--   if hasMenuActiveMenuItem and activeWindow == "" then
--     windows[activeWindow]:isVisible(false)
--   end

--   -- activate and save that window
--   if hasMenuActiveMenuItem then
--     activeWindow = windows[item]
--     windows[activeWindow]:isVisible(true)
--     return true
--   end

--   -- if user clicks on `Accept`
--   if item == 5 then
--     return parseOptionstoCFG(event)
--   end

--   -- if user clicks on `Cancel`
--   if item == 6 then
--     setDefaultOptions()
--     window:isVisible(false)
--     event:sendStateChange(States.OptionsMenuClose)
--     return true
--   end

--   return false
-- end

-- -- Add the event handler for the options menu, where
-- -- `event` is the Input::Event class in C++
-- window:setInputHandler(function (event)
--   if not window:isVisible() or window:isAnimating() then
--     return
--   end

--   local isInside = window:isInside(event:position())
--   local isMousePress = event:type() == InputEvent.MousePress

--   -- if click is outside
--   if isMousePress and not isInside then
--     window:isVisible(false)
--     e:sendStateChange(States.OptionsMenuClose)
--     e:stopPropgation()
--   end

--   -- let all window items get their chance at the event
--   window:defaultInputHandler(event)

--   -- these will only be true if they're clicked and the event
--   -- hasnt already been handled by window:defaultInputHandler
--   local isEnter = event:keyPressed(Glfw.Key.Enter)
--   local isClick = event:buttonPressed(Glfw.MouseButton.Left)

--   if (isEnter or isClick) and onClick(event) then
--     e:stopPropgation()
--     return
--   end

--   if event:keyPressed(Glfw.Key.Escape) then
--     window:isVisible(false)
--     event:sendStateChange(States.OptionsMenuClose)
--     event:stopPropgation()
--   end

--   -- Nothing else to do if the event hasnt been handled or if the previous type
--   -- wasnt KeyPress
--   if not event:hasBeenHandled() or event:prevType() ~= InputEvent.KeyPress then
--     return
--   end

--   -- handle the specific case where an inputbox has changed
--   -- and overriden some other keybindings. Need to clear that specific
--   -- keybinding other keybindings
--   local keyName = Glfw.Key.toString(event:key())
--   local inputBoxes = windows.Keybindings:inputboxes()
--   local changedKey = ""

--   for key, box in inputBoxes do
--     if box:hasChanged() then
--       keyChanged = key
--     end
--   end

--   if changedKey ~= "" then
--     for key, box in inputBoxes do
--       if key ~= keyChanged and box:text() == keyName then
--         box:changeText("unbound", true)
--       end
--     end
--   end
-- end)

-- OptionsMenu.mainWindow = window
OptionsMenu.windows = windows

return OptionsMenu
