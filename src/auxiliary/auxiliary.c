#include "lua.h"

static int pack(lua_State *L) {
	int i;
	int n = lua_gettop(L);
	lua_createtable(L, n, 1);
	lua_pushnumber(L, n);
	lua_rawseti(L, -2, 0);
	lua_insert(L, 1);
	for(i = 1; i <= n; i++) {
		lua_rawseti(L, 1, i);
	}
	return 1;
}

static int unpack(lua_State *L) {
	int i, n;
	lua_rawgeti(L, 1, 0);
	n = lua_tonumber(L, -1);
	lua_pop(L, 1);
	for(i = n; i >= 1; i--) {
		lua_rawgeti(L, 1, i);
	}
	lua_remove(L, 1);
	return n;
}

int luaopen_refser_auxiliary(lua_State *L) {
	lua_newtable(L);
	lua_pushstring(L, "pack");
	lua_pushcfunction(L, pack);
	lua_rawset(L, -3);
	lua_pushstring(L, "unpack");
	lua_pushcfunction(L, unpack);
	lua_rawset(L, -3);
	return 1;
}
