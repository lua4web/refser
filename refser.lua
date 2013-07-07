local _M = {}

local crefser = require "crefser"

function _M.save(x)
	return crefser.save(1/0, -1/0, 0/0, x)
end

function _M.load(s)
	return crefser.load(1/0, -1/0, 0/0, s)
end

function _M.assert(result, err)
	if err then
		error(err)
	else
		return result
	end
end

return _M
