local benchmark = require "benchmark"

local x = {}

for i = 1, 1000000 do
	x[i] = math.random()
end

benchmark(x, "numbers")
