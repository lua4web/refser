#include "saver.hpp"

#include <stdlib.h>
#include "format.hpp"
#include "writer.hpp"
#include "fixbuf.hpp"

Saver::Saver(Lua *L) {
	int maxtuple;
	this->L = L;
	this->count = 0;
	this->nesting = 0;
	this->items = 0;
	
	L->rawgeti(_SAVER_I_OPTS, 1);
	this->maxnesting = L->tonumber(-1);
	L->pop();
	
	L->rawgeti(_SAVER_I_OPTS, 2);
	maxtuple = L->tonumber(-1);
	L->pop();
	
	L->rawgeti(_SAVER_I_OPTS, 3);
	this->maxitems = L->tonumber(-1);
	L->pop();
	
	L->remove(_SAVER_I_OPTS);
	
	if(L->gettop() - _SAVER_I_X + 1 > maxtuple) {
		throw _SAVER_ERR_TOOLONG;
	}
	
	this->B = new FixBuf(L, _SAVER_I_BUFF);
}

Saver::~Saver() {
	delete this->B;
}

void Saver::process_number(int index) {
	lua_Number x = this->L->tonumber(index);
	this->B->addf(_FORMAT_NUMBER_MAX, "%.*g", _FORMAT_NUMBER_LEN, x);
}

void Saver::process_string(int index) {
	size_t len;
	const char *s = this->L->tolstring(index, &len);
	this->B->addquoted(s, len);
}

void Saver::process_table(int index) {
	lua_Number x;
	int i = 1;
	
	this->nesting++;
	if(this->nesting > this->maxnesting) {
		throw _SAVER_ERR_TOODEEP;
	}
	
	this->count++;
	this->L->pushvalue(index);
	this->L->pushnumber(this->count);
	this->L->rawset(_SAVER_I_REG);
	
	this->B->add(_FORMAT_TABLE_START);
				
	while(this->L->rawgeti(index, i++)) {
		this->process(this->L->gettop());
		this->L->pop();
	}
	
	this->B->add(_FORMAT_ARRAY_HASH_SEP);
	
	while(this->L->next(index)) {
		if((this->L->type(-2) != LUA_TNUMBER) || (!is_int(x = this->L->tonumber(-2))) || (x >= i) || (x <= 0)) {
			this->process(this->L->gettop() - 1);
			this->process(this->L->gettop());
		}
		this->L->pop();
	}
				
	this->B->add(_FORMAT_TABLE_END);
	
	this->nesting--;
}

// adds value at index to buffer
// stack-balanced
void Saver::process(int index) {
	this->items++;
	if(this->items > this->maxitems) {
		throw _SAVER_ERR_ITEMS;
	}
	if(!this->L->checkstack(2)) {
		throw _SAVER_ERR_STACK;
	}
	switch(this->L->type(index)) {
		case LUA_TNIL: {
			this->B->add(_FORMAT_NIL);
			break;
		}
		case LUA_TBOOLEAN: {
			if(this->L->toboolean(index)) {
				this->B->add(_FORMAT_TRUE);
			}
			else {
				this->B->add(_FORMAT_FALSE);
			}
			break;
		}
		case LUA_TNUMBER: {
			if(this->L->rawequal(index, index)) {
				if(this->L->rawequal(index, _SAVER_I_INF)) {
					this->B->add(_FORMAT_INF);
				}
				else {
					if(this->L->rawequal(index, _SAVER_I_MINF)) {
						this->B->add(_FORMAT_MINF);
					}
					else {
						this->B->add(_FORMAT_NUMBER);
						this->process_number(index);
					}
				}
			}
			else {
				this->B->add(_FORMAT_NAN);
			}
			break;
		}
		case LUA_TSTRING: {
			this->process_string(index);
			break;
		}
		case LUA_TTABLE: {
			this->L->pushvalue(index);
			this->L->rawget(_SAVER_I_REG);
			if(this->L->isnil(-1)) {
				this->L->pop();
				this->process_table(index);
			}
			else {
				this->B->add(_FORMAT_TABLE_REF);
				this->process_number(this->L->gettop());
				this->L->pop();
			}
			break;
		}
		case LUA_TFUNCTION: {
			throw _SAVER_ERR_FUNCTION;
			break;
		}
		case LUA_TUSERDATA: {
			throw _SAVER_ERR_USERDATA;
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			throw _SAVER_ERR_USERDATA;
			break;
		}
		case LUA_TTHREAD: {
			throw _SAVER_ERR_THREAD;
			break;
		}
	}
}

void Saver::pushresult() {
	this->B->pushresult();
}
