#ifndef SAVER_H
#define SAVER_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "fixbuf.h"

#define _SAVER_I_INF 1
#define _SAVER_I_MINF 2
#define _SAVER_I_MAXNESTING 3
#define _SAVER_I_X 4
#define _SAVER_I_REG 5
#define _SAVER_I_BUFF 6

#define _SAVER_ERR_TOODEEP 1
#define _SAVER_ERR_NONTRIVIAL 2

typedef struct saver {
	lua_State *L;
	fixbuf *B;
	int count;
	int maxnesting;
} saver;

// initializes saver
void saver_init(saver *S, lua_State *L, int maxnesting);

// adds value at index to buffer
// stack-balanced
// returns 0 or error code
int saver_process(saver *S, int index, int nesting);

#endif
