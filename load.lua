--[[ TODO: 
1. Implement load() without loadstring()
]]

local function load(s)
	return loadstring("return "..s)()
end

return load
