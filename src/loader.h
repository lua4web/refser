#ifndef LOADER_H
#define LOADER_H

#include "luapp.h"
#include "fixbuf.h"

#define _LOADER_I_INF 1
#define _LOADER_I_MINF 2
#define _LOADER_I_NAN 3
#define _LOADER_I_REG 4
#define _LOADER_I_BUFF 5
#define _LOADER_I_X 6

#define _LOADER_ERR_TOODEEP 1
#define _LOADER_ERR_MAILFORMED 2
#define _LOADER_ERR_STACK 3
#define _LOADER_ERR_ITEMS 4

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
		
		void eat();
		void eat(size_t size);
		int process_number();
		int process_string();
		int process_table();
	public:
		int maxtuple;
		Loader(Lua *L);
		~Loader();
		int process(int role);
		int done();
};

#endif
