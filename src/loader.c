#include "loader.h"

#include <stdlib.h>
#include "format.h"

#define ensure(cond) if(!(cond)) { \
	return _LOADER_ERR_MAILFORMED; \
}

#define eat_byte(LO) LO->s++; \
LO->len--;

#define eat_bytes(LO, count) LO->s += count; \
LO->len -= count;

// initializes loader
void loader_init(loader *LO, lua_State *L, const char *s, size_t len) {
	LO->L = L;
	LO->B = malloc(sizeof *LO->B);
	fixbuf_init(L, LO->B, _LOADER_I_BUFF);
	LO->count = 0;
	LO->s = s;
	LO->len = len;
}

static int loader_process_number(loader *LO) {
	ensure(LO->len);
	size_t i = 0;
	while(LO->s[i] != _FORMAT_NUMBER_DELIM) {
		i++;
		ensure(LO->len > i);
	}
	lua_pushlstring(LO->L, LO->s, i);
	eat_bytes(LO, i + 1);
	lua_Number x = lua_tonumber(LO->L, -1);
	lua_pop(LO->L, 1);
	lua_pushnumber(LO->L, x);
	return 0;
}

static int loader_process_string(loader *LO) {
	fixbuf_reset(LO->B);
	ensure(LO->len);
	char esc;
	size_t i = 0;
	while(LO->s[i] != '"') {
		if(LO->s[i] == '\\') {
			ensure(LO->len > i + 1);
			switch(LO->s[i+1]) {
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
					return _LOADER_ERR_MAILFORMED;
					break;
				}
			}
			fixbuf_addlstring(LO->B, LO->s, i);
			fixbuf_addchar(LO->B, esc);
			eat_bytes(LO, i + 2);
			i = 0;
		}
		else {
			i++;
		}
		ensure(LO->len > i);
	}
			
	fixbuf_addlstring(LO->B, LO->s, i);
			
	eat_bytes(LO, i + 1);
			
	fixbuf_pushresult(LO->B);			
	return 0;
}

static int loader_process_table(loader *LO) {
	lua_newtable(LO->L);
	lua_pushvalue(LO->L, -1);
	lua_rawseti(LO->L, _LOADER_I_REG, ++LO->count);
	
	ensure(LO->len);
			
	int err;
	int i = 0;
			
	while(*LO->s != _FORMAT_ARRAY_HASH_SEP) {
		if(err = loader_process(LO)) {
			return err;
		}
		lua_rawseti(LO->L, -2, ++i);
		ensure(LO->len);
	}
			
	eat_byte(LO);
	ensure(LO->len);
			
	while(*LO->s != _FORMAT_TABLE_END) {
		if(err = loader_process(LO)) {
			return err;
		}
		if(err = loader_process(LO)) {
			return err;
		}
		lua_rawset(LO->L, -3);
		ensure(LO->len);
	}
			
	eat_byte(LO);
	return 0;
}

// reads next value from string
// puts it on top of lua stack
// returns 0 or error code
int loader_process(loader *LO) {
	ensure(LO->len);
	if(!lua_checkstack(LO->L, 2)) {
		return _LOADER_ERR_TOODEEP;
	}
	eat_byte(LO);
	switch(LO->s[-1]) {
		case _FORMAT_NIL: {
			lua_pushnil(LO->L);
			break;
		}
		case _FORMAT_TRUE: {
			lua_pushboolean(LO->L, 1);
			break;
		}
		case _FORMAT_FALSE: {
			lua_pushboolean(LO->L, 0);
			break;
		}
		case _FORMAT_INF: {
			lua_pushvalue(LO->L, _LOADER_I_INF);
			break;
		}
		case _FORMAT_MINF: {
			lua_pushvalue(LO->L, _LOADER_I_MINF);
			break;
		}
		case _FORMAT_NAN: {
			lua_pushvalue(LO->L, _LOADER_I_NAN);
			break;
		}
		case _FORMAT_TABLE_REF: {
			int err;
			if(err = loader_process_number(LO)) {
				return err;
			}
			lua_Number x = lua_tonumber(LO->L, -1);
			lua_pop(LO->L, 1);
			ensure(is_int(x) && x <= LO->count && x >= 1);
			lua_rawgeti(LO->L, _LOADER_I_REG, x);
			break;
		}
		case _FORMAT_TABLE_START: {
			return loader_process_table(LO);
			break;
		}
		case _FORMAT_NUMBER: {
			return loader_process_number(LO);
			break;
		}
		case _FORMAT_STRING: {
			return loader_process_string(LO);
			break;
		}
		default: {
			return _LOADER_ERR_MAILFORMED;
			break;
		}
	}
	return 0;
}
