local _M = {}

local crefser = require "crefser"

_M.maxnesting = 250

function _M.save(x)
	return crefser.save(1/0, -1/0, _M.maxnesting, x)
end

function _M.load(s)
	return crefser.load(1/0, -1/0, 0/0, _M.maxnesting, s)
end

function _M.assert(result, err)
	if err then
		error(err)
	else
		return result
	end
end

return _M
