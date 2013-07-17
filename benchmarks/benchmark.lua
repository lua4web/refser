local refser = require "refser"

refser.maxitems = 10 ^ 9

local function benchmark(x, title)
	print("Starting benchmark " .. title)
	
	collectgarbage()
	
	local memx = collectgarbage "count"
	local memstart = collectgarbage "count"
	
	io.write "  refser.save: "
	local start = os.clock()
	local s = assert(refser.save(x))
	print(os.clock() - start)
	local memend = collectgarbage "count"
	
	collectgarbage()
	local memfinal = collectgarbage "count"
	
	print("    Memory consumed(raw): " .. (memend - memstart))
	print("    Memory consumed(clean): " .. (memfinal - memstart))
	print("    Output length: " .. #s)
	
	memstart = collectgarbage "count"
	
	print()
	io.write "  refser.load: "
	start = os.clock()
	local y = assert(refser.load(s))
	print(os.clock() - start)
	memend = collectgarbage "count"
	collectgarbage()
	memfinal = collectgarbage "count"
	
	print("    Memory consumed(raw): " .. (memend - memstart))
	print("    Memory consumed(clean): " .. (memfinal - memstart))
	
	s = nil
	y = nil
	collectgarbage()
	print()
	print("  Memory consumed(y - x): " .. (collectgarbage "count" - memx))
	
	print "Benchmark completed"
	print()
end

return benchmark
