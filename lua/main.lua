
local drawable = {}

function addtoDrawable(item, name)
  if name ~= nil then
    drawable[name] = item
  else
    table.insert(drawable, item)
  end
end

function removefromDrawable(name)
  if drawable[name] ~= nil then
    drawable[name] = nil
  end
end

function draw()
  for k, v in pairs(drawable) do
    v:draw()
  end
end

function update(deltaTime)
  for k, v in pairs(drawable) do
    v:update(deltaTime)
  end
end

if cfg.console.enabled then
  print('Loading console')
  require 'console/main'
end
