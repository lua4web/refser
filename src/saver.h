#ifndef SAVER_H
#define SAVER_H

#include "lua.h"
#include "lauxlib.h"
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


typedef struct saver {
	lua_State *L;
	fixbuf *B;
	int count;
	int maxnesting;
	int items;
	int maxitems;
} saver;

// initializes saver
void saver_init(saver *S, lua_State *L, int maxnesting, int maxitems);

// adds value at index to buffer
// stack-balanced
// returns 0 or error code
int saver_process(saver *S, int index, int nesting);

// prepares saver for freeing
void saver_free(saver *S);

#endif
