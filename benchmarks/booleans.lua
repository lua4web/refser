local benchmark = require "benchmark"

local x = {}

for i = 1, 10000000 do
	x[i] = (i % 2) == 0
end

benchmark(x, "booleans")
