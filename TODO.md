# TODO

* Improve passing data from lua refser to c part. 
* Change `refser.crefser` module to `refser.c`. 
* Implement a table format with explicit context link: `#(id)[keys]|[key-value pairs]}`
* Implement worker:setmaxitems(), worker:setmaxnesting(), etc. 
* Implement worker:resave(...): save in a new context, but use old context for referencing with explicit context linking. 
* Add more tests for worker functions. 
* Rewrite README.md in readable English. 
