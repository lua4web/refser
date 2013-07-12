#include <stdlib.h>
#include "lua.h"
#include "lauxlib.h"
#include "fixbuf.h"
#include "saver.h"
#include "loader.h"

static int save(lua_State *L) {
	saver *S;
	int err;
	int maxnesting, maxtuple;
	int i;
	int tuplesize = lua_gettop(L) - _SAVER_I_MAXTUPLE;
	lua_newtable(L);
	maxnesting = lua_tonumber(L, _SAVER_I_MAXNESTING);
	lua_replace(L, _SAVER_I_REG);
	maxtuple = lua_tonumber(L, _SAVER_I_MAXTUPLE);
	if(tuplesize > maxtuple) {
		lua_pushnil(L);
		lua_pushstring(L, "refser.save error: tuple is too long");
		return 2;
	}
	
	S = malloc(sizeof *S);
	saver_init(S, L, maxnesting);
	
	for(i = 0; i < tuplesize; i++) {
		if(err = saver_process(S, _SAVER_I_X + i, 0)) {
			lua_settop(L, 0);
			lua_pushnil(L);
			switch(err) {
				case _SAVER_ERR_TOODEEP: {
					lua_pushstring(L, "refser.save error: table is too deep");
					break;
				}
				case _SAVER_ERR_STACK: {
					lua_pushstring(L, "refser.save error: lua stack exhausted");
					break;
				}
				case _SAVER_ERR_FUNCTION: {
					lua_pushstring(L, "refser.save error: attempt to save function");
					break;
				}
				case _SAVER_ERR_USERDATA: {
					lua_pushstring(L, "refser.save error: attempt to save userdata");
					break;
				}
				case _SAVER_ERR_THREAD: {
					lua_pushstring(L, "refser.save error: attempt to save thread");
					break;
				}
			}
			return 2;
		}
	}
	fixbuf_pushresult(S->B);
	saver_free(S);
	free(S);
	return 1;
}

static int load(lua_State *L) {
	size_t len;
	const char *s;
	loader *LO;
	int err;
	int tuplesize;
	int maxnesting, maxtuple;
	maxnesting = lua_tonumber(L, _LOADER_I_MAXNESTING);
	lua_newtable(L);
	lua_replace(L, _LOADER_I_REG);
	maxtuple = lua_tonumber(L, _LOADER_I_MAXTUPLE);

	s = luaL_checklstring(L, _LOADER_I_X, &len);
	LO = malloc(sizeof *LO);
	loader_init(LO, L, s, len, maxnesting);
	
	tuplesize = 0;
	while(LO->len && tuplesize < maxtuple) {
		if(err = loader_process(LO, _LOADER_ROLE_NONE, 0)) {
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
				case _LOADER_ERR_STACK: {
					lua_pushstring(L, "refser.load error: lua stack exhausted");
					break;
				}
				default: {
					lua_pushstring(L, "refser.load error: unknown error");
					break;
				}
			}
			return 2;
		}
		tuplesize++;
	}
	if(LO->len) {
		lua_pushnil(L);
		lua_pushstring(L, "refser.load error: tuple is too long");
		return 2;
	}
	lua_pushnumber(L, tuplesize);
	lua_replace(L, _LOADER_I_X);
	loader_free(LO);
	free(LO);
	return tuplesize + 1;
}

int luaopen_crefser (lua_State *L) {
	lua_newtable(L);
	lua_pushstring(L, "save");
	lua_pushcfunction(L, save);
	lua_rawset(L, -3);
	lua_pushstring(L, "load");
	lua_pushcfunction(L, load);
	lua_rawset(L, -3);
	return 1;
}

