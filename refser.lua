local _M = {}

local save = require("save").save

function _M.save(x)
	return save((x), 1/0, 0/0)
end

return _M
