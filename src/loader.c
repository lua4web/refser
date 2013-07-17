#include "loader.h"

#include <stdlib.h>
#include "format.h"

#define ensure(cond) { \
	if(!(cond)) { \
		return _LOADER_ERR_MAILFORMED; \
	} \
}

#define eat_byte(LO) { \
	(LO)->s++; \
	(LO)->len--; \
}

#define eat_bytes(LO, count) { \
	(LO)->s += (count); \
	LO->len -= (count); \
}

#define valid_number_char(c) (((c) <= '9' && (c) >= '0') || (c) == 'e' || (c) == '.' || (c) == '-')

// initializes loader
void loader_init(loader *LO, lua_State *L, const char *s, size_t len, int maxnesting, int maxitems) {
	LO->L = L;
	LO->B = malloc(sizeof *LO->B);
	fixbuf_init(L, LO->B, _LOADER_I_BUFF);
	LO->count = 0;
	LO->s = s;
	LO->len = len;
	LO->maxnesting = maxnesting;
	LO->items = 0;
	LO->maxitems = maxitems;
}

static int loader_process_number(loader *LO) {
	size_t i = 0;
	lua_Number x;
	ensure(LO->len);
	while(valid_number_char(LO->s[i])) {
		i++;
	}
	ensure(i <= _FORMAT_NUMBER_MAX);
	x = strtod(LO->s, NULL);
	ensure(x || (i == 1 && LO->s[0] == '0'));
	eat_bytes(LO, i);
	lua_pushnumber(LO->L, x);
	if(lua_rawequal(LO->L, -1, -1)) {
		return 0;
	}
	else {
		return _LOADER_ERR_MAILFORMED;
	}
}

static int loader_process_string(loader *LO) {
	char esc;
	size_t i = 0;
	fixbuf_reset(LO->B);
	ensure(LO->len);
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

static int loader_process_table(loader *LO, int nesting) {
	int err;
	int i = 1;
	
	if(nesting > LO->maxnesting) {
		return _LOADER_ERR_TOODEEP;
	}
	
	lua_newtable(LO->L);
	lua_pushvalue(LO->L, -1);
	lua_rawseti(LO->L, _LOADER_I_REG, ++LO->count);
	
	ensure(LO->len);
	
	while(*LO->s != _FORMAT_ARRAY_HASH_SEP) {
		if(err = loader_process(LO, _LOADER_ROLE_VALUE, nesting)) {
			return err;
		}
		lua_rawseti(LO->L, -2, i++);
		ensure(LO->len);
	}
			
	eat_byte(LO);
	ensure(LO->len);
			
	while(*LO->s != _FORMAT_TABLE_END) {
		if(err = loader_process(LO, _LOADER_ROLE_KEY, nesting)) {
			return err;
		}
		if(err = loader_process(LO, _LOADER_ROLE_VALUE, nesting)) {
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
int loader_process(loader *LO, int role, int nesting) {
	if(++LO->items > LO->maxitems) {
		return _LOADER_ERR_ITEMS;
	}
	ensure(LO->len);
	if(!lua_checkstack(LO->L, 2)) {
		return _LOADER_ERR_STACK;
	}
	eat_byte(LO);
	switch(LO->s[-1]) {
		case _FORMAT_NIL: {
			ensure(role == _LOADER_ROLE_NONE);
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
			ensure(role != _LOADER_ROLE_KEY);
			lua_pushvalue(LO->L, _LOADER_I_NAN);
			break;
		}
		case _FORMAT_TABLE_REF: {
			int err;
			lua_Number x;
			if(err = loader_process_number(LO)) {
				return err;
			}
			x = lua_tonumber(LO->L, -1);
			lua_pop(LO->L, 1);
			ensure(is_int(x) && x <= LO->count && x >= 1);
			lua_rawgeti(LO->L, _LOADER_I_REG, x);
			break;
		}
		case _FORMAT_TABLE_START: {
			return loader_process_table(LO, nesting + 1);
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

// prepares loader for freeing
void loader_free(loader *LO) {
	free(LO->B);
}
