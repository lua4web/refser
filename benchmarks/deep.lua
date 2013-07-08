--[[

local benchmark = require "benchmark"

local x = {}

local xx = x

for i = 1, 100000 - 1 do
	xx[1] = {}
	xx = xx[1]
end

benchmark(x, "deep")

]]

-- this one generated segfault. Unknown reason. 
-- fix: restrict nesting level

local benchmark = require "benchmark"

local x = {}

local xx = x

for i = 1, 10000 - 1 do
	xx[1] = {}
	xx = xx[1]
end

benchmark(x, "deep")
