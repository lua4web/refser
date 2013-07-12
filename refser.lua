local _M = {}

local crefser = require "crefser"

_M.maxnesting = 250

_M.maxtuple = 20

_M.maxitems = 10 ^ 6

function _M.save(...)
	return crefser.save(1/0, -1/0, _M.maxnesting, _M.maxtuple, _M.maxitems, ...)
end

function _M.load(s)
	return crefser.load(1/0, -1/0, 0/0, _M.maxnesting, _M.maxtuple, _M.maxitems, s)
end

return _M
