# TODO

* Improve passing data from lua refser to c part. 
* Replace `doublecontext` option with `mode` option: similarly to metamethod __mode, it may contain "s" and "l" to enable saving and loading. 
* Implement a table format with explicit context link: `#(id)[keys]|[key-value pairs]}`
* Implement worker:resave(...): save in a new context, but use old context for referencing with explicit context linking. 
* Rewrite README.md in readable English. 
