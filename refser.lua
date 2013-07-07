local _M = {}

local save = require("save").save

function _M.save(x)
	return save(1/0, -1/0, x)
end

return _M
