#include "saver.hpp"

#include <stdlib.h>
#include "format.hpp"
#include "writer.hpp"
#include "fixbuf.hpp"

Saver::Saver(Lua *L) {
	int maxtuple;
	this->L = L;
	this->nesting = 0;
	this->items = 0;
	
	L->getfield(_SAVER_I_REG, "n");
	this->count = L->tonumber(-1);
	L->pop();
	
	L->getfield(_SAVER_I_OPTS, "maxnesting");
	this->maxnesting = L->tonumber(-1);
	L->pop();
	
	L->getfield(_SAVER_I_OPTS, "maxtuple");
	maxtuple = L->tonumber(-1);
	L->pop();
	
	L->getfield(_SAVER_I_OPTS, "maxitems");
	this->maxitems = L->tonumber(-1);
	L->pop();
	
	L->getfield(_SAVER_I_OPTS, "doublecontext");
	this->doublecontext = L->toboolean(-1);
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

void Saver::process_number(lua_Number x) {
	this->B->addf(_FORMAT_NUMBER_MAX, "%.*g", _FORMAT_NUMBER_LEN, x);
}

void Saver::process_string(const char *s, size_t len) {
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
	this->L->settable(_SAVER_I_REG);
	
	if(this->doublecontext) {
		this->L->pushvalue(index);
		this->L->settablei(_SAVER_I_REG, this->count);
	}
	
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
			this->B->add(this->L->toboolean(index) ? _FORMAT_TRUE : _FORMAT_FALSE);
			break;
		}
		case LUA_TNUMBER: {
			lua_Number x = this->L->tonumber(index);
			if(isnan(x)) {
				this->B->add(_FORMAT_NAN);
			}
			else if(isinf(x)) {
				this->B->add((x > 0) ? _FORMAT_INF : _FORMAT_MINF);
			}
			else {
				this->B->add(_FORMAT_NUMBER);
				this->process_number(x);
			}
			break;
		}
		case LUA_TSTRING: {
			size_t len;
			const char *s = this->L->tolstring(index, &len);
			this->process_string(s, len);
			break;
		}
		case LUA_TTABLE: {
			this->L->pushvalue(index);
			this->L->gettable(_SAVER_I_REG);
			if(this->L->isnil(-1)) {
				this->L->pop();
				this->process_table(index);
			}
			else {
				lua_Number x = this->L->tonumber(this->L->gettop());
				this->B->add(_FORMAT_TABLE_REF);
				this->process_number(x);
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
	this->L->pushnumber(this->count);
	this->L->setfield(_SAVER_I_REG, "n");
	this->B->pushresult();
}
