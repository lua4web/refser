require "lunit"

local refser  = require "refser"

local function random_string()
	local len = math.random(1, 100)
	local s = {}
	for i = 1, len do
		s[i] = string.char(math.random(48, 125))
	end
	return table.concat(s)
end

local function random_trivial()
	local t = math.random(1, 5)
	if t == 1 then
		return nil
	elseif t == 2 then
		return (math.random(1, 2) == 1)
	elseif t == 3 then
		return math.huge * math.random(-1, 1)
	elseif t == 4 then
		return math.random(1, 100000)
	elseif t == 5 then
		return math.random() * math.random(-50, 50)
	else
		return random_string()
	end
end

module("trivial", lunit.testcase, package.seeall)

function test_nil()
	local x = nil
	local s = refser.save(x)
	assert_string(s)
	assert_equal("n", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_true()
	local x = true
	local s = refser.save(x)
	assert_string(s)
	assert_equal("T", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_false()
	local x = false
	local s = refser.save(x)
	assert_string(s)
	assert_equal("F", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_inf()
	local x = 1/0
	local s = refser.save(x)
	assert_string(s)
	assert_equal("I", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_minf()
	local x = -1/0
	local s = refser.save(x)
	assert_string(s)
	assert_equal("i", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_nan()
	local x = 0/0
	local s = refser.save(x)
	assert_string(s)
	assert_equal("N", s)
	local y = refser.load(s)
	assert_not_equal(y, y)
end

function test_integer()
	local x = math.random(1, 100000)
	local s = refser.save(x)
	assert_string(s)
	assert_equal("D"..tostring(x).."#", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_number()
	local x = math.random() * math.random(-50, 50)
	local s = refser.save(x)
	assert_string(s)
	assert_equal("D"..string.format("%.17g", x).."#", s)
	local y = refser.load(s)
	assert_equal(x, y)
end

function test_string()
	local x = random_string()
	local s = refser.save(x)
	assert_string(s)
	assert_equal(string.format("%q", x), s)
	local y = refser.load(s)
	assert_equal(x, y)
end

module("tables", lunit.testcase, package.seeall)

function test_empty()
	local x = {}
	local s = refser.save(x)
	assert_string(s)
	assert_equal("{|}", s)
	local y = refser.load(s)
	assert_nil(next(y))
end

function test_array()
	local x = {}
	for i = 1, 100 do
		x[i] = random_trivial()
	end
	local s = refser.save(x)
	assert_string(s)
	y = refser.load(s)
	assert_table(y)
end

function test_table()
	local x = {}
	for i = 1, 100 do
		local index = random_trivial()
		if (index == nil) or (index ~= index) then
			index = 0
		end
		x[index] = random_trivial()
	end
	local s = refser.save(x)
	assert_string(s)
	y = refser.load(s)
	assert_table(y)
end

function test_recursive()
	local x = {}
	x[x] = x
	local s = refser.save(x)
	assert_string(s)
	assert_equal("{|@1#@1#}", s)
	local y = refser.load(s)
	assert_table(y)
	assert_equal(y, y[y])
end

function test_depth()
	local x = {}

	local xx = x

	for i = 1, 250 - 1 do
		xx[1] = {}
		xx = xx[1]
	end
	local s = refser.save(x)
	assert_string(s)
	local y = refser.load(s)
	assert_table(y)
end

module("maxnesting", lunit.testcase, package.seeall)

function test_toodeep()
	local x = {}

	local xx = x

	for i = 1, 250 do
		xx[1] = {}
		xx = xx[1]
	end
	
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: table is too deep", err)
	
	refser.maxnesting = 251
	
	s = refser.save(x)
	assert_string(s)
	
	local y = refser.load(s)
	assert_table(y)
	
	refser.maxnesting = 250
	
	y, err = refser.load(s)
	assert_nil(y)
	assert_equal("refser.load error: table is too deep", err)
end

module("nontrivial", lunit.testcase, package.seeall)

function test_function()
	local x = function() end
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: attempt to save non-trivial data", err)
end

function test_userdata()
	local x = io.open("test.lua", "r")
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: attempt to save non-trivial data", err)
end

function test_thread()
	local x = coroutine.create(function() end)
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: attempt to save non-trivial data", err)
end

module("mailformed", lunit.testcase, package.seeall)

function test_empty()
	local s = ""
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_empty_table()
	local s = "{}"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_extra()
	local s = [["foobar"]].."\0"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_number_short()
	local s = "D123"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_number_corrupted()
	local s = "Dfoo#"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_number_long()
	local s = "D11111111111111111111111111111111111111111111111111111111111111#"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_empty_ref()
	local s = "@1#"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_strange_ref()
	local s = "{@1.00000000000000000000000000000000000000000000000000000000000000000000000#|}"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_nil_key()
	local s = "{|nD1#}"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

function test_nan_key()
	local s = "{|ND1#}"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end
