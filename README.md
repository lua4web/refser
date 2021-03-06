# refser - Lua table serialization library

refser is yet another Lua table serialization library mostly written in C++. 

Features:

* Can save and load nil, booleans, numbers, strings, tables and tuples of them. 
* Output is endian-portable and binary safe. 
* Recursive tables and cross-references are handled correctly. 
* Options to restrict saved and loaded data at run-time. 
* Options to persist context across several operations. 

## Installation

Install refser using [luarocks](http://luarocks.org): 

```
luarocks install refser
```

Last version uploaded to luarocks is v0.2. 

## Usage

1. Include refser:

	```lua
	local refser = require "refser"
	```

	Note: plain `require "refser"` doesn't work. 

2. Save data and check for errors:

	```lua
	local output, err = refser.save(...)
	if not output then
		-- error handling
	else
		-- ...
	end
	```

3. Load data and check for errors:

	```lua
	local count, ... = refser.load(input)
	if not count then
		-- error handling
	else
		-- ...
	end
	```

## Restriction

refser provides several methods to restrict saved and loaded data. 

* Nesting level of tables can't be larger than [refser.maxnesting](#refsermaxnesting). 
* Tuple length can't be larger than [refser.maxtuple](#refsermaxtuple). 
* Number of items can't be larger than [refser.maxitems](#refsermaxitems). 

## Reference

### refser.save(...)

Saves tuple of Lua values into string and returns it. In case of error returns `nil` plus error message. 

Output is binary safe, it can not contain newlines and embedded zeros. 

```lua
print(refser.save([[Newlines
and embedded]].."\0zeros"))
-- "Newlines\nand embedded\zzeros"
```

#### Identity-preserving table serialization

refser.save preserves all references in table. 

```lua
x = {}
x[x] = x
s = refser.save(x)
ok, y = refser.load(s)
assert(y == y[y]) -- OK
```

### refser.load(s)

Loads tuple from string `s`. Returns length of loaded tuple and tuple itself. In case of error returns `nil` plus error message. 

### refser.maxnesting

This variable sets maximum nesting level for saved and loaded tables. Default value is `250`. It can be changed at run-time to suit user's needs. 

```lua
x = {{{}}}
refser.maxnesting = 2
assert(refser.save(x)) -- refser.save error: table is too deep
refser.maxnesting = 3
assert(refser.save(x)) -- OK
```

### refser.maxtuple

This variable sets maximum tuple length for saved and loaded tuples. Default value is `20`. It can be changed at run-time to suit user's needs. 

```lua
a, b, c = "foo", "bar", "baz"
refser.maxtuple = 2
assert(refser.save(a, b, c)) -- refser.save error: tuple is too long
refser.maxtuple = 3
assert(refser.save(a, b, c)) -- OK
```

### refser.maxitems

This variable sets maximum number of items for saved and loaded tuples. Default value is `1000000`. It can be changed at run-time to suit user's needs. All values are considered items. 

```lua
a = {} -- 1 item
a[a] = a -- +2 items; both keys and values count
b = "foo" -- +1 item
c = {a} -- +2 items; in array part of table only values count
-- Total items: 6
refser.maxitems = 5
assert(refser.save(a, b, c)) -- refser.save error: too many items
refser.maxitems = 6
assert(refser.save(a, b, c)) -- OK
```

### refser.new(options)

Returns a refser worker object which may be used for a number of save/load operations in the same context. 

`options` argument should be nil or a table. Possible fields:

#### maxnesting

Sets maximum nesting level for all operations of the worker. Overrides [refser.maxnesting](#refsermaxnesting). 

#### maxtuple

Sets maximum tuple length for all operations of the worker. Overrides [refser.maxtuple](#refsermaxtuple). 

#### maxitems

Sets maximum items count for all operations of the worker. Overrides [refser.maxitems](#refsermaxitems). 

#### context

When specified, refser will use provided table as context instead of a new, empty context. 
Context determines which tables are recognised by refser. New records may be added into context during save and load. 

Example:

```lua

x = {1, 2, 3, 4, 5}
c = {[x] = 1, n = 1} -- context table includes x, its id is 1, size of context is 1

worker = refser.new{
	context = c
}

y = {x}
s = worker:save(y)
print(s) -- prints {@1|} - x is recognised and not serialized

```

#### mode

A string defining which operations worker can perform. 

If character "s" can be found in the string, saving is enabled; if "l" is present, loading is enabled. 

Default value is "sl". 

###	worker:save(...)

Similar to [refser.save](#refsersave), but uses worker's options and context. 

###	worker:load(...)

Similar to [refser.load](#refserloads), but uses worker's options and context. 

###	worker:setoptions(options)

Sets options of worker in the same way it is done on initialization. 

###	worker:setcontext(context)

Sets context of worker. 

###	worker:setmaxnesting(maxnesting)

Sets maxnesting option. 

###	worker:setmaxtuples(maxtuples)

Sets maxtuples option. 

###	worker:setmaxitems(maxitems)

Sets maxitems option. 

## Using refser for continious exchange of data

This section is work in process. 

---

Example:

```lua

-- worker of first state
worker1 = refser.new()

-- worker of second state
worker2 = refser.new()

-- first state creates and sends data

x1 = {}
y1 = {}
x1[x1] = y1
y1[x1] = x1

s1 = worker1:save(x1, y1)

-- send(s1)

-- second state receives data and sends back some new data

ok, x2, y2 = worker2:load(s1)

z2 = {[x2] = y2}

s2 = worker2:save(z2)

-- send(s2)

-- first state receives data

ok, z1 = worker1:load(s2)

assert(z1[x1] == y1) -- OK

```

## Output format

Output format is developed to be easily read by computer, not human, but it still can be used for debugging purposes, if necessary. 

* `nil` is saved as `n`. 
* `true` and `false` are saved as `T` and `F`, respectively. 
* `math.huge` and `-math.huge` are saved as `I` and `i`, respectively. 
* `NaN` is saved as `N`. 
* numbers are saved using `tostring`, with additional `D` in the beginning. 
* strings are saved using `string.format("%q")`. 
* tables' contents are saved between curly braces, with array part separated from hash part by `|`. There are no separators between values in array part, or between key and values in hash part, or between key-value pairs. 
* references are saved as `@` plus ID of corresponding table(without `D` in the beginning). Tables receive their IDs in the order `refser.save` meets them. 
* tuples are saved as sequence of values. 

### Examples

* An empty tuple. 

	```lua
	print(refser.save()) -- prints nothing
	print(string.len(refser.save())) -- 0
	```

* A single value. 

	```lua
	print(refser.save("foo")) -- "foo"
	```

* Three values. 

	```lua
	print(refser.save("foo", "bar", "baz")) -- "foo""bar""baz"
	```

* An empty table. No array part, no hash part. 

	```lua
	x = {}
	print(refser.save(x)) -- {|}
	```

* An array with 3 numbers. 

	```lua
	x = {1, 2, 3}
	print(refser.save(x)) -- {D1D2D3|}
	```

* One string in array part and a key-value pair. 

	```lua
	x = {"foo", bar = "baz"}
	print(refser.save(x)) -- {"foo"|"bar""baz"}
	```

* Nested tables. 

	```lua
	x = {{}, [{}] = {}}
	print(refser.save(x)) -- {{|}|{|}{|}}
	```

* A table with self-references. 

	```lua
	x = {}
	x[x] = x
	print(refser.save(x)) -- {|@1@1}
	```

* A more complicated example of cross-references. 

	```lua
	x = {}
	y = {}
	y[y] = y
	x[x] = y
	print(refser.save(x)) -- {|@1{|@2@2}}
	```

	`@1` is `x` and `@2` is `y`. 

* A tuple with cross-references. 

	```lua
	x = {}
	print(refser.save(x, x, {x})) -- {|}@1{@1|}
	```

## License

Copyright © 2013 lua4web <lua4web@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
