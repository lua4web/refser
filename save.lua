--[[ TODO: 
1. Errors should be returned as second result with nil
2. Localize used global functions
3. Test issues with string.format
]]

local function isint(x)
	return type(x) == "number" and x == math.floor(x)
end

local function trivial(x)
	if type(x) == "number" then
		return tostring(x)
	elseif type(x) == "string" then
		return string.format("%q", x) 
	elseif type(x) == "boolean" then
		return x and "true" or "false"
	end
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
		next_is_key = true
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
	if trivial(x) then
		add(saver, trivial(x))
	else
		if type(x) == "table" then
			pushtable(saver, x)
		else
			error("Attempt to serialize non-trivial type")
		end
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
			if saver.top.next_is_key then
				saver.top.next_is_key = false
				saver.top.curkey = next(saver.top.table, saver.top.curkey)
				if saver.top.curkey ~= nil then
					commize(saver)
					add(saver, "[")
					process(saver, saver.top.curkey)
				else
					poptable(saver)
				end
			else
				saver.top.next_is_key = true
				add(saver, "]=")
				process(saver, saver.top.table[saver.top.curkey])
			end
		until saver.stack_depth == 0
	end
	
	return table.concat(saver.res)
end

return save
