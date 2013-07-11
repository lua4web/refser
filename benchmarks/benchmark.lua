local refser = require "refser"

local function benchmark(x, title)
	print("Starting benchmark "..title)
	
	collectgarbage()
	
	io.write "refser.save: "
	local start = os.clock()
	local s = assert(refser.save(x))
	print(os.clock() - start)
	
	collectgarbage()
	
	io.write "refser.load: "
	start = os.clock()
	local y = assert(refser.load(s))
	print(os.clock() - start)
	
	print "Benchmark completed"
end

return benchmark
