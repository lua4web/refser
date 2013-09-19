#ifndef LOADER_H
#define LOADER_H

#include "lua.hpp"
#include "fixbuf.hpp"

#define _LOADER_I_REG 1
#define _LOADER_I_BUFF 2
#define _LOADER_I_OPTS 3
#define _LOADER_I_X 3

#define _LOADER_ERR_TOODEEP "refser.load error: table is too deep"
#define _LOADER_ERR_MAILFORMED "refser.load error: mailformed input"
#define _LOADER_ERR_STACK "refser.load error: lua stack exhausted"
#define _LOADER_ERR_ITEMS "refser.load error: too many items"
#define _LOADER_ERR_CONTEXT "refser.load error: broken context reference"

#define _LOADER_ROLE_NONE 1
#define _LOADER_ROLE_KEY 2
#define _LOADER_ROLE_VALUE 3

class Loader {
	private:
		Lua *L;
		FixBuf *B;
		const char *s;
		size_t len;
		int count;
		int nesting;
		int maxnesting;
		int items;
		int maxitems;
		int doublecontext;
		
		void eat();
		void eat(size_t size);
		void process_number();
		void process_string();
		void process_table();
	public:
		int maxtuple;
		Loader(Lua *L);
		~Loader();
		void process(int role);
		int done();
		void pushresult();
};

#endif
