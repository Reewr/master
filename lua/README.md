# Lua API

Lua is a large part of our engine. It is, for instance, used to initiate all the GUI elements. It's used to run commands in the console and its used to interact with the engine during runtime.

The API tries to be consistent in that it tries to expose as few global variables as possible. Please try to keep it like this and declare any variable you use as `local`. In order to allow for very few globals, the API uses a `require`-based system where you ask for what you need.

## Global variables

Firstly though, lets go over the global variables that actually exist:

- `cfg`
- `console`

These two is the only two global variables. `cfg` contains all the settings that the user has within their config file and `console` is defined *if* `cfg.console.enabled` is true. You should therefore always make sure that you know that `console` is active before trying to use it.

## Global functions

There are some global functions that are expose to help with rendering and updating elements. Currently, this is only supported for GUI elements.

- `addtoDrawable(item, name=index)`
- `removefromDrawable(name)`

These two functions allow you to add items to a list of items that should be drawn and updated. The item is expected to have a `update` and `draw` function.

## Require-able Libraries

In order to utilize the libraries that the engine exposes, you'll need to `require` them. In addition to being able to require specific files, you may also use any of the paths described below. All the paths follow a namespace to limit the amount of names it'll use. They all use capitalized letters and you can import any part of the namespace.

### GUI

- GUI
- GUI.Dropdown
- GUI.Slider
- GUI.Inputbox
- GUI.Menu
- GUI.Menu.Settings
- GUI.Menu.Orientation
- GUI.Text
- GUI.Text.Color
- GUI.Text.Style
- GUI.Window

If you require `GUI`, you will get all the elements, but you can specify specifically what elements you want to load. There are some special elements in that list, which are :

- 'GUI.Menu.Settings'
- 'GUI.Menu.Orientation'
- 'GUI.Text.Color'
- 'GUI.Text.Style'

These elements are not real types. Orientation, Color and Style are enum types that can be imported and used. Settings is a type that helps to create the Menus.

### Math

There is only one type which is exported from the math library for now:

- Math
- Math.vec2

### Shape

Shape only has one type which is exported. This may be extended:

- Shape
- Shape.Rectangle

### Input

Input also only have one type exported, but with an addition enum type

- Input
- Input.Event
- Input.Event.Type

## Example

What follows is an example of how to create a text element that is centered on the screen.

```lua
-- start my requiring the needed libraries
local Text = require 'GUI.Text'
local vec2 = require 'Math.vec2'

local myText = Text.new("Font::Dejavu", "This is some Text", vec2.new(0, 0))
local size   = myText:size()

-- Adjust the position based on the size of the text
local newPos = vec2.new(cfg.graphics.res.x - size.x / 2,
                        cfg.graphics.res.y - size.y / 2)

myText:setPosition(newPos)
addtoDrawable(myText)
```
