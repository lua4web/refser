local benchmark = require "benchmark"

local x = {}

for i = 1, 1000000 do
	x[i] = 1/0
	if (i % 2) == 0 then
		x[i] = x[i] * (-1)
	end
end

benchmark(x, "infs")
