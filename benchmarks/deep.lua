local benchmark = require "benchmark"

local x = {}

local xx = x

for i = 1, 250 - 1 do
	xx[1] = {}
	xx = xx[1]
end

benchmark(x, "deep")
