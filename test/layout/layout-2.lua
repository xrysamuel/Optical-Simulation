function calculateX(y)
  return math.sqrt((3 + y * y) / 3)
end

local startY = -3
local endY = 3
local step = 0.01

local x = {}
local y = {}

for i = startY, endY, step do
  local currentY = i
  local currentX = calculateX(currentY)
  
  table.insert(x, currentX)
  table.insert(y, currentY)
end

for i = 1, #x-1 do
  add_refractive(-x[i], y[i], -x[i+1], y[i+1], 1.5, 1.0)
end

add_refractive(-2.0, 3.0, -2.0, -3.0, 1.5, 1.0)


for i = -1.0, 1.0, 0.1 do
    add_lightray(-4.0, i, 1.0, 0.0)
end