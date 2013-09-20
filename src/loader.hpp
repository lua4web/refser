#ifndef LOADER_H
#define LOADER_H

#include "lua.hpp"
#include "worker.hpp"

#define _LOADER_ERR_TOODEEP "refser.load error: table is too deep"
#define _LOADER_ERR_MAILFORMED "refser.load error: mailformed input"
#define _LOADER_ERR_STACK "refser.load error: lua stack exhausted"
#define _LOADER_ERR_ITEMS "refser.load error: too many items"
#define _LOADER_ERR_CONTEXT "refser.load error: broken context reference"

#define _LOADER_ROLE_NONE 1
#define _LOADER_ROLE_KEY 2
#define _LOADER_ROLE_VALUE 3

class Loader: public Worker {
	private:
		const char *s;
		size_t len;
		
		void eat();
		void eat(size_t size);
		void process_number();
		void process_string();
		void process_table();
	public:
		int maxtuple;
		explicit Loader(Lua *L) : Worker(L) {
			this->s = this->L->tolstring(_I_X, &this->len);
		}
		void process(int role);
		int done();
		int pushresult();
};

#endif
