--[[ TODO: 
1. Catch recursive tables
]]

local type = type
local next = next
local tostring = tostring
local format = string.format
local concat = table.concat
local huge = math.huge
local floor = math.floor

local function notint(x)
	return type(x) ~= "number" or floor(x) ~= x
end

local function add(saver, s)
	saver.res_len = saver.res_len + 1
	saver.res[saver.res_len] = s
end

local function pushtable(saver, t)
	add(saver, "{")
	saver.stack_depth = saver.stack_depth + 1
	saver.stack[saver.stack_depth] = {
		table = t,
		comma = false,
		curkey = nil,
		next_is_key = true,
		indexing = true,
		i = 1
	}
	saver.top = saver.stack[saver.stack_depth]
end

local function poptable(saver)
	add(saver, "}")
	saver.stack[saver.stack_depth] = nil
	saver.stack_depth = saver.stack_depth - 1
	saver.top = saver.stack[saver.stack_depth]
end

local function process(saver, x)
	if type(x) == "number" then
		if x == huge then
			add(saver, "1/0")
		elseif x == -huge then
			add(saver, "-1/0")
		elseif x ~= x then
			add(saver, "0/0")
		else
			add(saver, tostring(x))
		end
	elseif type(x) == "string" then
		add(saver, format("%q", x))
	elseif type(x) == "boolean" then
		add(saver, x and "true" or "false")
	elseif type(x) == "table" then
		pushtable(saver, x)
	else
		return true
	end
end

local function commize(saver)
	if saver.top.comma then
		add(saver, ",")
	else
		saver.top.comma = true
	end
end

local function save(x)
	local saver = {}
	saver.res = {}
	saver.res_len = 0
	saver.stack = {}
	saver.stack_depth = 0
	
	process(saver, x)
	
	if type(x) == "table" then
		repeat
			if saver.top.indexing then
				if saver.top.table[saver.top.i] ~= nil then
					saver.top.i = saver.top.i + 1
					commize(saver)
					if process(saver, saver.top.table[saver.top.i - 1]) then
						return nil, "Attempt to serialize non-trivial data"
					end
				else
					saver.top.indexing = false
				end
			else
				if saver.top.next_is_key then
					saver.top.curkey = next(saver.top.table, saver.top.curkey)
					if saver.top.curkey ~= nil then
						if notint(saver.top.curkey) or saver.top.curkey < 1 or saver.top.curkey >= saver.top.i then
							saver.top.next_is_key = false
							commize(saver)
							add(saver, "[")
							if process(saver, saver.top.curkey) then
								return nil, "Attempt to serialize non-trivial data"
							end
						end
					else
						poptable(saver)
					end
				else
					saver.top.next_is_key = true
					add(saver, "]=")
					if process(saver, saver.top.table[saver.top.curkey]) then
						return nil, "Attempt to serialize non-trivial data"
					end
				end
			end
		until saver.stack_depth == 0
	end
	
	return concat(saver.res)
end

return save
