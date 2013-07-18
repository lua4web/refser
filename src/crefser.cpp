#include <stdlib.h>
#include "luapp.h"
#include "fixbuf.h"
#include "saver.h"
#include "loader.h"

static int save(lua_State *LS) {
	Lua *L = new Lua(LS);
	Saver *S = new Saver(L);
	if(!S->ok) {
		L->settop(0);
		L->pushnil();
		L->pushstring("refser.save error: tuple is too long");
		return 2;
	}
	
	int err;
	int i;
	int tuplesize = L->gettop() - _SAVER_I_X + 1;
	
	for(i = 0; i < tuplesize; i++) {
		if(err = S->process(_SAVER_I_X + i)) {
			L->settop(0);
			L->pushnil();
			switch(err) {
				case _SAVER_ERR_TOODEEP: {
					L->pushstring("refser.save error: table is too deep");
					break;
				}
				case _SAVER_ERR_STACK: {
					L->pushstring("refser.save error: lua stack exhausted");
					break;
				}
				case _SAVER_ERR_FUNCTION: {
					L->pushstring("refser.save error: attempt to save function");
					break;
				}
				case _SAVER_ERR_USERDATA: {
					L->pushstring("refser.save error: attempt to save userdata");
					break;
				}
				case _SAVER_ERR_THREAD: {
					L->pushstring("refser.save error: attempt to save thread");
					break;
				}
				case _SAVER_ERR_ITEMS: {
					L->pushstring("refser.save error: too many items");
					break;
				}
			}
			return 2;
		}
	}
	S->pushresult();
	delete L;
	delete S;
	return 1;
}

static int load(lua_State *LS) {
	Lua *L = new Lua(LS);
	Loader *LO;
	int err;
	LO = new Loader(L);
	
	int tuplesize = 0;
	while(!LO->done() && tuplesize < LO->maxtuple) {
		if(err = LO->process(_LOADER_ROLE_NONE)) {
			L->settop(0);
			L->pushnil();
			switch(err) {
				case _LOADER_ERR_MAILFORMED: {
					L->pushstring("refser.load error: mailformed input");
					break;
				}
				case _LOADER_ERR_TOODEEP: {
					L->pushstring("refser.load error: table is too deep");
					break;
				}
				case _LOADER_ERR_STACK: {
					L->pushstring("refser.load error: lua stack exhausted");
					break;
				}
				case _LOADER_ERR_ITEMS: {
					L->pushstring("refser.load error: too many items");
					break;
				}
				default: {
					L->pushstring("refser.load error: unknown error");
					break;
				}
			}
			return 2;
		}
		tuplesize++;
	}
	if(!LO->done()) {
		L->settop(0);
		L->pushnil();
		L->pushstring("refser.load error: tuple is too long");
		return 2;
	}
	L->pushnumber(tuplesize);
	L->replace(_LOADER_I_X);
	delete L;
	delete LO;
	return tuplesize + 1;
}

extern "C" {
	int luaopen_crefser(lua_State *L) {
		lua_newtable(L);
		lua_pushstring(L, "save");
		lua_pushcfunction(L, save);
		lua_rawset(L, -3);
		lua_pushstring(L, "load");
		lua_pushcfunction(L, load);
		lua_rawset(L, -3);
		return 1;
	}
}
