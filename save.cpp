extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <stdlib.h>
#include <math.h>

using namespace std;

#include "addrreg.h"

#define VIEW_NIL 'n'
#define VIEW_NAN 'N'
#define VIEW_TABLE_START '{'
#define VIEW_TABLE_REF '@'
#define VIEW_TABLE_END '}'
#define VIEW_TRUE 'T'
#define VIEW_FALSE 'F'
#define VIEW_INF 'I'
#define VIEW_MINF 'i'
#define VIEW_SEP ','
#define VIEW_EQ '='
#define VIEW_HASH_SEP '|'

#define INDEX_INF 2
#define INDEX_MINF 3
#define INDEX_X 1

#define ERROR_NONTRIVIAL 1
#define ERROR_STACK_EXHAUSTED 2

#define is_int(x) (fmod(x, 1.0) == 0.0)

struct SaverNode {
	char next_is_key;
	char indexing;
	char comma;
	lua_Number i;
	SaverNode *next, *prev;
	
	SaverNode(SaverNode *last);
	SaverNode();
};

SaverNode::SaverNode(SaverNode *top) {
	this->next_is_key = 1;
	this->next = 0;
	this->prev = top;
	this->comma = 0;
	this->indexing = 1;
	this->i = 1;
}

SaverNode::SaverNode() {
	this->next = this->prev = 0;
}

class Saver {
	public:
		lua_State *L;
		luaL_Buffer *Buf;
		AddrReg addrreg;
		SaverNode *top;
		Saver(lua_State *L);
		
		void pushResult();
		void add();
		void add(char c);
		void add(const char *s);
		void add(const char *s, size_t l);
		void sep();
		void pushTable(int index);
		void popTable();
		void process(int index);
};

Saver::Saver(lua_State *L) {
	this->L = L;
	this->Buf = new luaL_Buffer;
	luaL_buffinit(L, this->Buf);
	this->top = new SaverNode;
}

void Saver::pushResult() {
	luaL_pushresult(this->Buf);
}

void Saver::add() {
	luaL_addvalue(this->Buf);
}

void Saver::add(char c) {
	luaL_addchar(this->Buf, c);
}

void Saver::add(const char *s) {
	luaL_addstring(this->Buf, s);
}

void Saver::add(const char* s, size_t l) {
	luaL_addlstring(this->Buf, s, l);
}

void Saver::sep() {
	if(this->top->comma) {
		this->add(this->top->comma);
	}
	this->top->comma = VIEW_SEP;
}

void Saver::pushTable(int index) {
	this->add(VIEW_TABLE_START);
	this->top->next = new SaverNode(this->top);
	this->top = this->top->next;
	if(!lua_checkstack(this->L, 5)) {
		throw ERROR_STACK_EXHAUSTED;
	}
	lua_pushvalue(this->L, index);
	lua_pushnil(this->L);
	lua_pushnil(this->L);
}

void Saver::popTable() {
	lua_pop(this->L, 1);
	this->add(VIEW_TABLE_END);
	this->top = this->top->prev;
	free(this->top->next);
}

void Saver::process(int index) {
	int type = lua_type(this->L, index);
	switch(type) {
		case LUA_TNIL: {
			this->add(VIEW_NIL);
			break;
		}
		case LUA_TBOOLEAN: {
			if(lua_toboolean(this->L, index)) {
				this->add(VIEW_TRUE);
			}
			else {
				this->add(VIEW_FALSE);
			}
			break;
		}
		case LUA_TSTRING: {
			size_t len;
			const char *s = lua_tolstring(this->L, index, &len);
			this->add(s, len);
			break;
		}
		case LUA_TNUMBER: {
			if(!lua_equal(this->L, index, index)) {
				this->add(VIEW_NAN);
			}
			else {
				if(lua_equal(this->L, index, INDEX_INF)) {
					this->add(VIEW_INF);
				}
				else {
					if(lua_equal(this->L, index, INDEX_MINF)) {
						this->add(VIEW_MINF);
					}
					else {
						lua_pushvalue(this->L, index);
						this->add();
					}
				}
			}
			break;
		}
		case LUA_TTABLE: {
			intptr_t addr = (intptr_t) lua_topointer(this->L, index);
			unsigned int id = this->addrreg.insert(addr);
			if(id) {
				this->add(VIEW_TABLE_REF);
				lua_pushnumber(this->L, id);
				this->add();
			}
			else {
				this->pushTable(index);
			}
			break;
		}
		default: {
			throw ERROR_NONTRIVIAL;
			break;
		}
	}
}

static int save(lua_State *L) {
	Saver *S;
	try {
		S = new Saver(L);	
		S->process(INDEX_X);
		
		while(S->top->prev) {
			if(S->top->indexing) {
				int i = lua_gettop(L);
				lua_rawgeti(L, -3, S->top->i);
				if(!lua_isnil(L, -1)) {
					S->top->i++;
					S->sep();
					S->process(-1);
				}
				else {
					S->top->indexing = 0;
					S->top->comma = VIEW_HASH_SEP;
				}
				lua_remove(L, i);
			}
			else {
				if(S->top->next_is_key) {
					S->top->next_is_key = 0; 
					// -1 = v, -2 = k, -3 = t
					lua_pop(L, 1); 
					// -1 = k, -2 = t
					if(lua_next(L, -2)) { // -1 = v, -2 = k, -3 = t
						if(lua_type(L, -2) == LUA_TNUMBER) {
							lua_Number x = lua_tonumber(L, -2);
							if(is_int(x) && x < S->top->i && x > 0) {
								S->top->next_is_key = 1; 
							}
							else { // TODO: clean this up
								S->sep();
								S->process(-2); // process key
							}
						}
						else {
							S->sep();
							S->process(-2); // process key
						}
					}
					else { // -1 = t
						S->popTable(); // ???
					}
				}
				else {
					S->top->next_is_key = 1;
					// -1 = v, -2 = k, -3 = t
					S->add(VIEW_EQ);
					S->process(-1);
				}
			}
		}
		
		S->pushResult();
		free(S);
		return 1;
	}
	
	catch(int error) {
		lua_pushnil(S->L);
		switch(error) {
			case ERROR_NONTRIVIAL: {
				lua_pushstring(S->L, "refser error: attempt to serialize non-trivial data");
				break;
			}
			case ERROR_STACK_EXHAUSTED: {
				lua_pushstring(S->L, "refser error: lua stack exhausted");
				break;
			}
			default: {
				lua_pushstring(S->L, "refser error: unknown");
				break;
			}
		}
		free(S);
		return 2;
	};
}

static const struct luaL_reg savelib[] = {
	{"save", save},
	{0, 0}
};

extern "C" {

	int luaopen_save (lua_State *L) {
		luaL_register(L, "save", savelib);
		return 1;
	}
}
