local refser = {}

refser.c = require "refser.c"
local class = require "30log"

refser.maxnesting = 250
refser.maxtuple = 20
refser.maxitems = 10 ^ 6

local worker = class()

function worker:__init(options)
	self.maxnesting = refser.maxnesting
	self.maxtuple = refser.maxtuple
	self.maxitems = refser.maxitems
	self:setoptions(options)
end

function worker:setoptions(options)
	options = options or {}
	self.maxnesting = options.maxnesting or self.maxnesting
	self.maxtuple = options.maxtuple or self.maxtuple
	self.maxitems = options.maxitems or self.maxitems
	self:setcontext(options.context)
	if options.doublecontext ~= nil then
		self.doublecontext = options.doublecontext
	end
end

function worker:setcontext(context)
	context = context or {}
	local mt = getmetatable(context) or {}
	if not mt.__mode then
		mt.__mode = "kv"
		setmetatable(context, mt)
	end
	if not context.n then
		context.n = 0
	end
	self.context = context
	return context
end

function worker:setmaxnesting(maxnesting)
	self.maxnesting = maxnesting
end

function worker:setmaxtuple(maxtuple)
	self.maxtuple = maxtuple
end

function worker:setmaxitems(maxitems)
	self.maxitems = maxitems
end

function worker:save(...)
	if select("#", ...) > self.maxtuple then
		return nil, "refser.save error: tuple is too long"
	end
	return refser.c.save(self, ...)
end

function worker:load(s)
	assert(type(s) == "string", ("bad argument #1 to 'load' (string expected, got %s"):format(type(s)))
	return refser.c.load(self, s)
end

function refser.new(options)
	return worker(options)
end

function refser.save(...)
	return worker():save(...)
end

function refser.load(s)
	return worker():load(s)
end

return refser
