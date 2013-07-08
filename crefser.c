#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "fixbuf.h"
#include "saver.h"
#include "loader.h"

static int save(lua_State *L) {
	lua_newtable(L);
	saver *S = malloc(sizeof *S);
	saver_init(S, L);
	int err;
	if(err = saver_process(S, _SAVER_I_X)) {
		lua_settop(L, 0);
		lua_pushnil(L);
		switch(err) {
			case _SAVER_ERR_NONTRIVIAL: {
				lua_pushstring(L, "refser.save error: attempt to save non-trivial data");
				break;
			}
			case _SAVER_ERR_TOODEEP: {
				lua_pushstring(L, "refser.save error: table is too deep");
				break;
			}
			default: {
				lua_pushstring(L, "refser.save error: unknown error");
				break;
			}
		}
		return 2;
	}
	fixbuf_pushresult(S->B);
	return 1;
}

static int load(lua_State *L) {
	lua_newtable(L);
	size_t len;
	const char *s = luaL_checklstring(L, _LOADER_I_X, &len);
	loader *LO = malloc(sizeof *LO);
	loader_init(LO, L, s, len);
	int err;
	if(err = loader_process(LO)) {
		lua_settop(L, 0);
		lua_pushnil(L);
		switch(err) {
			case _LOADER_ERR_MAILFORMED: {
				lua_pushstring(L, "refser.load error: mailformed input");
				break;
			}
			case _LOADER_ERR_TOODEEP: {
				lua_pushstring(L, "refser.load error: table is too deep");
				break;
			}
			default: {
				lua_pushstring(L, "refser.load error: unknown error");
				break;
			}
		}
		return 2;
	}
	if(LO->len) {
		lua_pushnil(L);
		lua_pushstring(L, "refser.load error: mailformed input");
		return 2;
	}
	return 1;
}

static const struct luaL_reg crefserlib[] = {
	{"save", save},
	{"load", load},
	{NULL, NULL}
};

int luaopen_crefser (lua_State *L) {
	luaL_register(L, "crefser", crefserlib);
	return 1;
}
