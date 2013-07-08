local benchmark = require "benchmark"

local x = {}

local function random_string()
	local len = math.random(1, 100)
	local s = {}
	for i = 1, len do
		s[i] = string.char(math.random(48, 125))
	end
	return table.concat(s)
end

for i = 1, 1000000 do
	x[i] = random_string()
end

benchmark(x, "strings")
