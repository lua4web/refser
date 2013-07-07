#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int load(lua_State *L) {
	lua_pushnil(L);
	lua_pushstring(L, "refser error: load() not implemented");
	return 2;
}

static const struct luaL_reg loadlib[] = {
	{"load", load},
	{NULL, NULL}
};

int luaopen_cload (lua_State *L) {
	luaL_register(L, "cload", loadlib);
	return 1;
}
