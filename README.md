# refser - fast serialization of tables with references

Allows to save primitive Lua types into strings and load them back. 

refser is similar to identity-preserving table serialization by Metalua([source](https://github.com/fab13n/metalua/blob/no-dll/src/lib/serialize.lua)), but it is much faster as it is written in C. 

## refser.save(x)

Saves `x` into string and returns it. In case of error returns `nil` plus error message. 

Output is binary safe, it can not contain newlines and embedded zeros. 

```lua
print(refser.save([[Newlines
and embedded]].."\0zeros"))
-- "Newlines\nand embedded\zzeros"
```

`refser.save` can save: 

* `nil`
* `boolean`
* `number`, including math.huge and NaN
* `string`
* `table`, including nested tables and tables with references

`refser.save` can't save:

* `function`
* `thread`
* `userdata`

### Identity-preserving table serialization

refser.save preserves all references between values in table. 

```lua
x = {}
x[x] = x
s = refser.save(x)
y = refser.load(s)
assert(y == y[y]) -- OK
```
	
## refser.load(s)

Loads data from string `s` and returns it. In case of error returns `nil` plus error message. 

Use `refser.assert` to catch errors. 

## refser.assert(...)

Use this function to catch errors from `refser.load`. Standart `assert` fails when expected data evaluates to false:

```lua
x = false
s = refser.save(x)
y = assert(refser.load(s)) -- assertation failed

y = refser.assert(refser.load(s)) -- OK
```

## License

Copyright © 2013 lua4web <lua4web@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
