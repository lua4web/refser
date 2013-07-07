# refser - yet another Lua table serialization library

refser aims to serialize nested tables with references correctly, similar to identity-preserving table serialization by Metalua, but faster and with shorter output. 

## Status

refser is under construction. 

Currently only refser.save() function is partially implemented.

## Internals

Output format:

1. Boolean true is "T", boolean false is "F". 
2. Numbers are serlialized using tostring. math.huge is "I", -math.huge is "i", Nan is "N". 
3. Strings are properly escaped. 
4. Tables are serialized in this way: first goes the array part separated with commas, then goes the hash part with keys and values separated with "=" and pairs itself separated with commas. Array and hash parts are separated with "|". 
5. References: tables which has already been met are presented by "@" plus its id. id of a table is N if it was the Nth table to be processed. 

Example:

    x={1}
    x[2] = x
    x[x] = 3
    x[3]={"indirect recursion", [x]=x}
    y = {x, x}
    x[y] = y

    print(refser.save(x))
    -- {1,@1,{"indirect recursion"|@1=@1}|@1=3,{@1,@1}=@3}

Here, @1 is the root table (x), @2 is table x[3] and @3 is y. 

## TODO

* Support saving tuples in refser.save()
* Improve numbers saving in refser.save()
* Implement refser.load()
