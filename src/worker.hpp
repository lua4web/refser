#ifndef WORKER_H
#define WORKER_H

#include "lua.hpp"
#include "fixbuf.hpp"

#define _I_SELF 1
#define _I_CONTEXT 2
#define _I_X 3

class Worker {
	protected:
		Lua *L;
		FixBuf *B;
		int count;
		int nesting;
		int maxnesting;
		int items;
		int maxitems;
		int maxtuple;
		int doublecontext;
	public:
		Worker(Lua *L);
		~Worker();
		virtual int pushresult();
};

#endif
