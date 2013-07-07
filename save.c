#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "fixbuf.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define _I_INF 1
#define _I_MINF 2
#define _I_X 3
#define _I_REG 4
#define _I_BUFF 5

#define _ERR_TOODEEP 1
#define _ERR_NONTRIVIAL 2

#define not_int(x) (fmod(x, 1.0) != 0.0)

static int process(fixbuf *B, int index, int *count) {
	if(!lua_checkstack(B->L, 2)) {
		return _ERR_TOODEEP;
	}
	switch(lua_type(B->L, index)) {
		case LUA_TNIL: {
			fixbuf_addchar(B, 'n');
			break;
		}
		case LUA_TBOOLEAN: {
			if(lua_toboolean(B->L, index)) {
				fixbuf_addchar(B, 'T');
			}
			else {
				fixbuf_addchar(B, 'F');
			}
			break;
		}
		case LUA_TNUMBER: {
			if(lua_equal(B->L, index, index)) {
				if(lua_equal(B->L, index, _I_INF)) {
					fixbuf_addchar(B, 'I');
				}
				else {
					if(lua_equal(B->L, index, _I_MINF)) {
						fixbuf_addchar(B, 'i');
					}
					else {
						lua_pushvalue(B->L, index);
						fixbuf_addvalue(B);
					}
				}
			}
			else {
				fixbuf_addchar(B, 'N');
			}
			break;
		}
		case LUA_TSTRING: {
			size_t len;
			const char *s = lua_tolstring(B->L, index, &len);
			fixbuf_addchar(B, '"');
			// TODO: escape special symbols
			fixbuf_addlstring(B, s, len);
			fixbuf_addchar(B, '"');
			break;
		}
		case LUA_TTABLE: {
			lua_pushvalue(B->L, index);
			lua_rawget(B->L, _I_REG);
			if(lua_isnil(B->L, -1)) {
				lua_pop(B->L, 1);
				
				lua_pushvalue(B->L, index);
				lua_pushnumber(B->L, ++*count);
				lua_rawset(B->L, _I_REG);
				
				int i = 1;
				lua_Number x;
				int err;
				char sep = '\0';
				
				fixbuf_addchar(B, '{');
				
				for(;;) {
					lua_rawgeti(B->L, index, i);
					if(lua_isnil(B->L, -1)) {
						sep = '|';
						break;
					}
					else {
						i++;
						if(sep) {
							fixbuf_addchar(B, sep);
						}
						else {
							sep = ',';
						}
						if(err = process(B, lua_gettop(B->L), count)) {
							return err;
						}
					}
					lua_pop(B->L, 1);
				}
				
				lua_pushnil(B->L);
				
				for(;;) {
					if(lua_next(B->L, index)) {
						if((lua_type(B->L, -2) != LUA_TNUMBER) || (not_int(x = lua_tonumber(B->L, -2))) || (x >= i) || (x <= 0)) {
							if(sep) {
								fixbuf_addchar(B, sep);
							}
							sep = ',';
							if(err = process(B, lua_gettop(B->L) - 1, count)) {
								return err;
							}
							fixbuf_addchar(B, '=');
							if(err = process(B, lua_gettop(B->L), count)) {
								return err;
							}
						}
						lua_pop(B->L, 1);
					}
					else {
						break;
					}
				}
				
				lua_pop(B->L, 1);
				
				fixbuf_addchar(B, '}');
			}
			else {
				fixbuf_addchar(B, '@');
				fixbuf_addvalue(B);
			}
			break;
		}
		default: {
			return _ERR_NONTRIVIAL;
			break;
		}
	}
	return 0;
}

static int save(lua_State *L) {
	lua_newtable(L);
	int count = 0;
	fixbuf *B = malloc(sizeof *B);
	fixbuf_init(L, B, _I_BUFF);
	int err;
	if(err = process(B, _I_X, &count)) {
		lua_settop(L, 0);
		lua_pushnil(L);
		switch(err) {
			case _ERR_NONTRIVIAL: {
				lua_pushstring(L, "refser error: attempt to save non-trivial data");
				break;
			}
			case _ERR_TOODEEP: {
				lua_pushstring(L, "refser error: table is too deep");
				break;
			}
			default: {
				lua_pushstring(L, "refser error: unknown error");
				break;
			}
		}
		return 2;
	}
	else {
		fixbuf_pushresult(B);
		return 1;
	}
}

static const struct luaL_reg savelib[] = {
	{"save", save},
	{NULL, NULL}
};

int luaopen_save (lua_State *L) {
	luaL_register(L, "save", savelib);
	return 1;
}
