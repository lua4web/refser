local _M = {}

local save = require("csave").save
local load = require("cload").load

function _M.save(x)
	return save(1/0, -1/0, x)
end

function _M.load(s)
	return load(1/0, -1/0, 0/0, s)
end

return _M
