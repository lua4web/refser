/* TODO:
1. Add array support
2. Check lua stack before pushing
3. Or better just emulate lua stack

gcc save.c --shared -I/usr/include/lua5.1 -o save.so
*/


#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef struct stack_element stack_element;

struct stack_element {
	int table;
	char comma;
	int curkey;
	char next_is_key;
	stack_element* next;
	stack_element* prev;
};

typedef struct saver saver;

struct saver {
	lua_State *L;
	stack_element* top;
	luaL_Buffer *Res;
};

static void add(saver *S, const char* str) {
	luaL_addstring(S->Res, str);
}

static void addl(saver *S, const char* str, size_t l) {
	luaL_addlstring(S->Res, str, l);
}

static void pushtable(saver *S, int i) {
	add(S, "{");
	if(S->top->next == NULL) {
		 S->top->next = malloc(sizeof *S->top->next);
	}
	S->top->next->table = i;
	S->top->next->comma = 0;
	S->top->next->curkey = 2;
	S->top->next->next_is_key = 1;
	S->top->next->next = NULL;
	S->top->next->prev = S->top;
	S->top = S->top->next;
}

static void poptable(saver *S) {
	add(S, "}");
	S->top = S->top->prev;
}

static char process(saver *S, int i) {
	int type = lua_type(S->L, i);
	switch(type) {
		case LUA_TBOOLEAN: {
			if(lua_toboolean(S->L, i)) {
				add(S, "true");
			}
			else {
				add(S, "false");
			}
			break;
		}
		case LUA_TSTRING: {
			size_t len;
			const char* str = lua_tolstring(S->L, i, &len);
			addl(S, str, len);
			break;
		}
		case LUA_TNUMBER: {
			if(!lua_equal(S->L, i, i)) {
				add(S, "0/0");
			}
			else {
				lua_pushvalue(S->L, i);
				const char* str = lua_tostring(S->L, -1);
				if(str[0] == 'i') {
					add(S, "1/0");
				}
				else {
					if(str[0] == '-') {
						if(str[1] == 'i') {
							add(S, "-1/0");
						}
						else {
							add(S, str);
						}
					}
					else {
						add(S, str);
					}
				}
				lua_pop(S->L, 1);
			}
			break;
		}
		case LUA_TTABLE: {
			pushtable(S, i);
			break;
		}
		default: {
			return 1;
			break;
		}
	}
	return 0;
}

static void commize(saver *S) {
	if(S->top->comma) {
		add(S, ",");
	}
	else {
		S->top->comma = 1;
	}
}

static int save(lua_State *L) {
	if(lua_isnone(L, 1)) {
		return 0;
	}
	if(lua_isnil(L, 1)) {
		lua_pushstring(L, "nil");
		return 1;
	}
	saver *S = malloc(sizeof *S);
	S->L = L;
	S->Res = malloc(sizeof *S->Res);
	luaL_buffinit(L, S->Res);
	
	S->top = malloc(sizeof *S->top);
	S->top->next = NULL;
	S->top->prev = NULL;
	
	lua_pushnil(L);
	
	if(process(S, 1)) {
		lua_pushnil(L);
		lua_pushstring(L, "Attempt to serialize non-trivial data1");
		return 2;
	}
	
	if(lua_istable(L, 1)) {
		do {
			if(S->top->next_is_key) {
				S->top->next_is_key = 0;
				lua_pushvalue(L, S->top->curkey); 
				if(lua_next(L, S->top->table)) {
					lua_pop(L, 1);
					S->top->curkey = lua_gettop(L);
					commize(S);
					add(S, "[");
					if(process(S, lua_gettop(L))) {
						lua_pushnil(L);
						lua_pushstring(L, "Attempt to serialize non-trivial data2");
						return 2;
					}
				}
				else {
					poptable(S);
				}
			}
			else {
				S->top->next_is_key = 1;
				add(S, "]=");
					
				lua_pushvalue(L, S->top->curkey); 
				lua_gettable(L, S->top->table);
				if(process(S, lua_gettop(L))) {
					lua_pushnil(L);
					lua_pushstring(L, "Attempt to serialize non-trivial data3");
					return 2;
				}
			}
		} while(S->top->prev != NULL);
	}
	
	luaL_pushresult(S->Res);
	return 1;
}
	
static const struct luaL_reg savelib[] = {
	{"save", save},
	{NULL, NULL}
};

int luaopen_save (lua_State *L) {
	luaL_register(L, "save", savelib);
	return 1;
}
