# refser - yet another Lua table serialization library

refser aims to serialize nested tables with references correctly, similar to identity-preserving table serialization by Metalua, but faster and with shorter output. 

## Status

refser is under testing and refactoring. 

## TODO

* Add documentation. 
* Add tests. 
* Submit refser as luarock.
* Support saving and loading tuples. 
* Improve string saving and loading(it seems to be bottleneck). 
* Make number serialization binary safe and shorter. 
* Merge csave.c and cload.c. 

