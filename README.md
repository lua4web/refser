# safeser - Lua table serialization library

## Features

* Serializes strings, numbers(including math.huge and NaN), booleans and non-recursive tables. 
* Serialization is stack-overflow safe: tables of any nesting depth can be serialized. 
* Serialization output is valid Lua expression. 
* Serialization returns nil plus error message if something goes wrong. 

## Not-implemented-yet features

* Serializes arrays properly. 
* Raises an error when finds recursive tables. 
* Errors can be ignored. 
* Deserialization doesn't use loadsting(), therefore, it is safe to use even for suspicious data. 
