#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "fixbuf.h"

#define _I_INF 1
#define _I_MINF 2
#define _I_NAN 3
#define _I_X 4
#define _I_REG 5
#define _I_BUFF 6

#define _ERR_TOODEEP 1
#define _ERR_NONTRIVIAL 2
#define _ERR_MAILFORMED 3

#define is_int(x) (fmod(x, 1.0) == 0.0)

#define ensure(cond) if(!(cond)) { \
	return _ERR_MAILFORMED; \
}

static int save_process(fixbuf *B, int index, int *count) {
	lua_Number x;
	
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
						fixbuf_addchar(B, 'D');
						x = lua_tonumber(B->L, index);
						fixbuf_addlstring(B, (const char *)&x, sizeof(lua_Number));
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
			fixbuf_addqstring(B, s, len);
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
				int err;
				
				fixbuf_addchar(B, '{');
				
				for(;;) {
					lua_rawgeti(B->L, index, i);
					if(lua_isnil(B->L, -1)) {
						fixbuf_addchar(B, '|');
						break;
					}
					else {
						i++;
						if(err = save_process(B, lua_gettop(B->L), count)) {
							return err;
						}
					}
					lua_pop(B->L, 1);
				}
				
				lua_pushnil(B->L);
				
				for(;;) {
					if(lua_next(B->L, index)) {
						if((lua_type(B->L, -2) != LUA_TNUMBER) || (!is_int(x = lua_tonumber(B->L, -2))) || (x >= i) || (x <= 0)) {
							if(err = save_process(B, lua_gettop(B->L) - 1, count)) {
								return err;
							}
							if(err = save_process(B, lua_gettop(B->L), count)) {
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
				x = lua_tonumber(B->L, -1);
				fixbuf_addlstring(B, (const char *)&x, sizeof(lua_Number));
				lua_pop(B->L, 1);
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
	if(err = save_process(B, _I_X, &count)) {
		lua_settop(L, 0);
		lua_pushnil(L);
		switch(err) {
			case _ERR_NONTRIVIAL: {
				lua_pushstring(L, "refser.save() error: attempt to save non-trivial data");
				break;
			}
			case _ERR_TOODEEP: {
				lua_pushstring(L, "refser.save() error: table is too deep");
				break;
			}
			default: {
				lua_pushstring(L, "refser.save() error: unknown error");
				break;
			}
		}
		return 2;
	}
	fixbuf_pushresult(B);
	return 1;
}

static int load_process(fixbuf *B, const char *s, size_t len, int *count, size_t *used) {
	lua_Number x;
	ensure(len);
	if(!lua_checkstack(B->L, 2)) {
		return _ERR_TOODEEP;
	}
	switch(*s) {
		case 'n': {
			*used = 1;
			lua_pushnil(B->L);
			break;
		}
		case 'T': {
			*used = 1;
			lua_pushboolean(B->L, 1);
			break;
		}
		case 'F': {
			*used = 1;
			lua_pushboolean(B->L, 0);
			break;
		}
		case 'I': {
			*used = 1;
			lua_pushvalue(B->L, _I_INF);
			break;
		}
		case 'i': {
			*used = 1;
			lua_pushvalue(B->L, _I_MINF);
			break;
		}
		case 'N': {
			*used = 1;
			lua_pushvalue(B->L, _I_NAN);
			break;
		}
		case '@': {
			*used = sizeof(lua_Number) + 1;
			ensure(len > sizeof(lua_Number));
			memcpy(&x, s + 1, sizeof(lua_Number));
			ensure(is_int(x) && x <= *count && x >= 1);
			lua_rawgeti(B->L, _I_REG, x);
			break;
		}
		case '{': {
			lua_newtable(B->L);
			lua_pushvalue(B->L, -1);
			lua_rawseti(B->L, _I_REG, ++*count);
			
			s++;
			len--;
			size_t curused;
			*used = 1;
			ensure(len);
			
			int err;
			int i = 0;
			
			while(*s != '|') {
				if(err = load_process(B, s, len, count, &curused)) {
					return err;
				}
				lua_rawseti(B->L, -2, ++i);
				s += curused;
				len -= curused;
				*used += curused;
				ensure(len);
			}
			
			s++;
			len--;
			(*used)++;
			ensure(len);
			
			while(*s != '}') {
				if(err = load_process(B, s, len, count, &curused)) {
					return err;
				}
				s += curused;
				len -= curused;
				*used += curused;
				if(err = load_process(B, s, len, count, &curused)) {
					return err;
				}
				lua_rawset(B->L, -3);
				s += curused;
				len -= curused;
				*used += curused;
				ensure(len);
			}
			
			(*used)++;
			break;
		}
		case 'D': {
			*used = sizeof(lua_Number) + 1;
			ensure(len > sizeof(lua_Number));
			memcpy(&x, s + 1, sizeof(lua_Number));
			lua_pushnumber(B->L, x);
			break;
		}
		case '"': {
			s++;
			len--;
			ensure(len);
			*used = 1;
			char esc;
			size_t i = 0;
			while(s[i] != '"') {
				if(s[i] == '\\') {
					ensure(len > i + 1);
					switch(s[i+1]) {
						case '\\': {
							esc = '\\';
							break;
						}
						case 'n': {
							esc = '\n';
							break;
						}
						case 'r': {
							esc = '\r';
							break;
						}
						case '"': {
							esc = '"';
							break;
						}
						case 'z': {
							esc = '\0';
							break;
						}
						default: {
							return _ERR_MAILFORMED;
							break;
						}
					}
					fixbuf_addlstring(B, s, i);
					fixbuf_addchar(B, esc);
					s += i + 2;
					len -= i + 2;
					*used += i + 2;
					i = 0;
				}
				else {
					i++;
				}
				ensure(len > i);
			}
			
			fixbuf_addlstring(B, s, i);
			
			*used += i + 1;
			
			fixbuf_pushresult(B);
			fixbuf_reset(B);
			
			break;
		}
		default: {
			return _ERR_MAILFORMED;
			break;
		}
	}
	return 0;
}

static int load(lua_State *L) {
	lua_newtable(L);
	size_t len;
	const char *s = luaL_checklstring(L, _I_X, &len);
	fixbuf *B = malloc(sizeof *B);
	fixbuf_init(L, B, _I_BUFF);
	int count = 0;
	size_t used;
	int err;
	if(err = load_process(B, s, len, &count, &used)) {
		lua_settop(L, 0);
		lua_pushnil(L);
		switch(err) {
			case _ERR_MAILFORMED: {
				lua_pushstring(L, "refser.load() error: mailformed input");
				break;
			}
			case _ERR_TOODEEP: {
				lua_pushstring(L, "refser.load() error: table is too deep");
				break;
			}
			default: {
				lua_pushstring(L, "refser.load() error: unknown error");
				break;
			}
		}
		return 2;
	}
	if(len != used) {
		lua_pushnil(L);
		lua_pushstring(L, "refser error.load(): mailformed input");
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
