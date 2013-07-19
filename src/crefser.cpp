#include <stdlib.h>
#include "lua.hpp"
#include "saver.hpp"
#include "loader.hpp"

static int save(lua_State *LS) {
	Lua L(LS);
	
	try {
		Saver S(&L);
		
		int i;
		int tuplesize = L.gettop() - _SAVER_I_X + 1;
		
		for(i = 0; i < tuplesize; i++) {
			S.process(_SAVER_I_X + i);
		}
		S.pushresult();
		return 1;
	}
	
	catch(const char *msg) {
		L.settop(0);
		L.pushnil();
		L.pushstring(msg);
		return 2;
	}
}

static int load(lua_State *LS) {
	Lua L(LS);
	
	try {
		Loader LO(&L);
		
		int tuplesize = 0;
		while(!LO.done() && tuplesize < LO.maxtuple) {
			LO.process(_LOADER_ROLE_NONE);
			tuplesize++;
		}
		if(!LO.done()) {
			L.settop(0);
			L.pushnil();
			L.pushstring("refser.load error: tuple is too long");
			return 2;
		}
		L.pushnumber(tuplesize);
		L.replace(_LOADER_I_X);
		return tuplesize + 1;
	}
	
	catch(const char *msg) {
		L.settop(0);
		L.pushnil();
		L.pushstring(msg);
		return 2;
	}
}

extern "C" {
	int luaopen_refser_crefser(lua_State *L) {
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
