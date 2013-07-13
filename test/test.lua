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
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_true()
	local x = true
	local s = refser.save(x)
	assert_string(s)
	assert_equal("T", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_false()
	local x = false
	local s = refser.save(x)
	assert_string(s)
	assert_equal("F", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_inf()
	local x = 1/0
	local s = refser.save(x)
	assert_string(s)
	assert_equal("I", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_minf()
	local x = -1/0
	local s = refser.save(x)
	assert_string(s)
	assert_equal("i", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_nan()
	local x = 0/0
	local s = refser.save(x)
	assert_string(s)
	assert_equal("N", s)
	local count, y = refser.load(s)
	assert_not_equal(y, y)
end

function test_integer()
	local x = math.random(1, 100000)
	local s = refser.save(x)
	assert_string(s)
	assert_equal("D"..tostring(x).."#", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_number()
	local x = math.random() * math.random(-50, 50)
	local s = refser.save(x)
	assert_string(s)
	assert_equal("D"..string.format("%.17g", x).."#", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_long()
	local x = -1234567898765432123456789
	x = x * 10 ^ (-130)
	local s = refser.save(x)
	assert_string(s)
	assert_equal("D"..string.format("%.17g", x).."#", s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

function test_string()
	local x = random_string()
	local s = refser.save(x)
	assert_string(s)
	assert_equal(string.format("%q", x), s)
	local count, y = refser.load(s)
	assert_equal(x, y)
end

module("tables", lunit.testcase, package.seeall)

function test_empty()
	local x = {}
	local s = refser.save(x)
	assert_string(s)
	assert_equal("{|}", s)
	local count, y = refser.load(s)
	assert_nil(next(y))
end

function test_array()
	local x = {}
	for i = 1, 100 do
		x[i] = random_trivial()
	end
	local s = refser.save(x)
	assert_string(s)
	local count, y = refser.load(s)
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
	local count, y = refser.load(s)
	assert_table(y)
end

function test_recursive()
	local x = {}
	x[x] = x
	local s = refser.save(x)
	assert_string(s)
	assert_equal("{|@1#@1#}", s)
	local count, y = refser.load(s)
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
	local count, y = refser.load(s)
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
	
	local count, y = refser.load(s)
	assert_table(y)
	
	refser.maxnesting = 250
	
	y, err = refser.load(s)
	assert_nil(y)
	assert_equal("refser.load error: table is too deep", err)
end

function test_refs_are_not_deep()
	local x = {}
	x[x] = x
	refser.maxnesting = 1
	local s, err = refser.save(x)
	assert_string(s)
	local count, y = refser.load(s)
	assert_table(y)
	assert_equal(y, y[y])
	refser.maxnesting = 250
end

module("nontrivial", lunit.testcase, package.seeall)

function test_function()
	local x = function() end
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: attempt to save function", err)
end

function test_userdata()
	local x = io.open("test.lua", "r")
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: attempt to save userdata", err)
end

function test_thread()
	local x = coroutine.create(function() end)
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: attempt to save thread", err)
end

module("mailformed", lunit.testcase, package.seeall)

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

function test_clever_nan_key()
	local s = "{|Dnan#T}"
	local data, err = refser.load(s)
	assert_nil(data)
	assert_equal("refser.load error: mailformed input", err)
end

module("tuples", lunit.testcase, package.seeall)

function test_empty()
	local s = refser.save()
	assert_string(s)
	assert_equal("", s)
	local count, data = refser.load(s)
	assert_nil(data)
	assert_equal(count, 0)
end

function test_several()
	local s = refser.save("foo", 12345, {}, true, nil)
	assert_string(s)
	assert_equal([["foo"D12345#{|}Tn]], s)
	local count, a, b, c, d, e = refser.load(s)
	assert_equal(count, 5)
	assert_equal("foo", a)
	assert_equal(12345, b)
	assert_nil(next(c))
	assert_true(d)
	assert_nil(e)
end

function test_tuple_refs()
	local x = {}
	local s = refser.save(x, nil, x, nil, {[x] = x}, nil)
	assert_string(s)
	assert_equal([[{|}n@1#n{|@1#@1#}n]], s)
	local count, a, b, c, d, e, f = refser.load(s)
	assert_equal(6, count)
	assert_nil(b)
	assert_nil(d)
	assert_nil(f)
	assert_equal(a, c)
	assert_equal(a, e[c])
end

module("maxtuple", lunit.testcase, package.seeall)

function test_long_tuple()
	local x = {}
	for i = 1, 21 do
		x[i] = i
	end
	local s, err = refser.save(table.unpack(x))
	assert_nil(s)
	assert_equal("refser.save error: tuple is too long", err)
	
	refser.maxtuple = 21
	
	local s, err = refser.save(table.unpack(x))
	assert_string(s)
	
	local function pack(count, ...)
		return count, table.pack(...)
	end
	
	local count, packed = pack(refser.load(s))
	assert_equal(21, count)
	for i = 1, 21 do
		assert_equal(i, packed[i])
	end
	
	refser.maxtuple = 20
end

function test_load_long_tuple()
	local s = string.rep("n", 100)
	local count, err = refser.load(s)
	assert_nil(count)
	assert_equal("refser.load error: tuple is too long", err)
end

function test_restricted()
	local username, password = "foobar", "qwerty"
	
	refser.maxtuple = 2
	refser.maxnesting = 0
	
	local s = refser.save(username, password)
	assert_string(s)
	assert_equal([["foobar""qwerty"]], s)
	
	local ok, username2, password2 = refser.load(s)
	
	assert_equal(2, ok)
	assert_equal(username, username2)
	assert_equal(password, password2)
	
	local attack1 = "{D12345#|TF}"
	local attack2 = [["foobar""qwerty""extra"]]
	
	assert_nil(refser.load(attack1))
	assert_nil(refser.load(attack2))
	
	refser.maxtuple = 20
	refser.maxnesting = 250
end

module("maxitems", lunit.testcase, package.seeall)

function test_many_items()
	local x = {}
	for i = 1, 1000 do
		x[i] = {}
		for j = 1, 1000 do
			x[i][j] = 0
		end
	end
	
	local s, err = refser.save(x)
	assert_nil(s)
	assert_equal("refser.save error: too many items", err)
	
	refser.maxitems = 1 + 1000 + 1000 * 1000
	
	s, err = refser.save(x)
	assert_string(s)
	
	refser.maxitems = 10 ^ 6
end

function test_combo()
	local a = {} -- 1
	a[a] = a -- 2, 3
	b = "foo" -- 4
	c = {a} -- 5, 6
	
	refser.maxitems = 5
	
	local s, err = refser.save(a, b, c)
	assert_nil(s)
	assert_equal("refser.save error: too many items", err)
	
	refser.maxitems = 6
	
	s, err = refser.save(a, b, c)
	assert_string(s)
	
	count, a2, b2, c2 = refser.load(s)
	assert_equal(3, count)
	assert_equal(a2, a2[a2])
	assert_equal(b, b2)
	assert_equal(a2, c2[1])
	
	refser.maxitems = 5
	
	ok, err = refser.load(s)
	assert_nil(ok)
	assert_equal("refser.load error: too many items", err)
	
	refser.maxitems = 10 ^ 6
end
