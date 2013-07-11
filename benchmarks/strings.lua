local benchmark = require "benchmark"

local x = {}

local function random_string()
	local len = math.random(1, 100)
	return string.rep("A", len)
end

for i = 1, 1000000 do
	x[i] = random_string()
end

benchmark(x, "strings")
