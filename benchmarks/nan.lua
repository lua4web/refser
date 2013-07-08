local benchmark = require "benchmark"

local x = {}

for i = 1, 10000000 do
	x[i] = 0/0
end

benchmark(x, "nan")
