local benchmark = require "benchmark"

local x = {}

local function nest(x, level)
	if level > 0 then
		level = level - 1
		for i = 1, 10 do
			x[i] = {}
			nest(x[i], level)
		end
	end
end

nest(x, 6)

benchmark(x, "nested")
