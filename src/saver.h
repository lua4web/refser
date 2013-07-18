#ifndef SAVER_H
#define SAVER_H

#include "luapp.h"
#include "fixbuf.h"

#define _SAVER_I_INF 1
#define _SAVER_I_MINF 2
#define _SAVER_I_MAXNESTING 3
#define _SAVER_I_MAXTUPLE 4
#define _SAVER_I_MAXITEMS 5
#define _SAVER_I_REG 3
#define _SAVER_I_BUFF 4
#define _SAVER_I_X 5

#define _SAVER_ERR_TOODEEP 1
#define _SAVER_ERR_STACK 2
#define _SAVER_ERR_FUNCTION 3
#define _SAVER_ERR_USERDATA 4
#define _SAVER_ERR_THREAD 5
#define _SAVER_ERR_ITEMS 6
#define _SAVER_ERR_UNKNOWN 100

class Saver {
	private:
		Lua *L;
		FixBuf *B;
		int count;
		int nesting;
		int maxnesting;
		int items;
		int maxitems;
		
		int process_number(int index);
		int process_string(int index);
		int process_table(int index);
	public:
		Saver(Lua *L, int maxnesting, int maxitems);
		~Saver();
		int process(int index);
		void pushresult();
};

#endif
