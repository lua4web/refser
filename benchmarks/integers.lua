local benchmark = require "benchmark"

local x = {}

for i = 1, 1000000 do
	x[i] = i
end

benchmark(x, "integers")
