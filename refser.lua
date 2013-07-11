local _M = {}

local crefser = require "crefser"

_M.maxnesting = 250

_M.maxtuple = 20

function _M.save(...)
	return crefser.save(1/0, -1/0, _M.maxnesting, _M.maxtuple, ...)
end

function _M.load(s)
	return crefser.load(1/0, -1/0, 0/0, _M.maxnesting, _M.maxtuple, s)
end

return _M
