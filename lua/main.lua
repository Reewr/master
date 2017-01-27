require 'console'

local drawable = {}
local index = 0

function addtoDrawable(item)
  drawable[index] = item
  index = index + 1
end

function draw()
  for k, v in pairs(drawable) do
    v:draw()
  end
end

function update()
end
