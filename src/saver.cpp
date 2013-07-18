#include "saver.h"

#include <stdlib.h>
#include "format.h"

Saver::Saver(Lua *L, int maxnesting, int maxitems) {
	this->L = L;
	this->B = new FixBuf(L, _SAVER_I_BUFF);
	this->count = 0;
	this->nesting = 0;
	this->maxnesting = maxnesting;
	this->items = 0;
	this->maxitems = maxitems;
}

Saver::~Saver() {
	delete this->B;
}

int Saver::process_number(int index) {
	int len;
	lua_Number x = this->L->tonumber(index);
	this->B->addf(_FORMAT_NUMBER_MAX, "%.*g", _FORMAT_NUMBER_LEN, x);
	return 0;
}

int Saver::process_string(int index) {
	size_t len;
	const char *s = this->L->tolstring(index, &len);
	this->B->addquoted(s, len);
	return 0;
}

int Saver::process_table(int index) {
	int err;
	lua_Number x;
	int i = 1;
	
	this->nesting++;
	if(this->nesting > this->maxnesting) {
		return _SAVER_ERR_TOODEEP;
	}
	
	this->count++;
	this->L->pushvalue(index);
	this->L->pushnumber(this->count);
	this->L->rawset(_SAVER_I_REG);
	
	this->B->add(_FORMAT_TABLE_START);
				
	while(this->L->rawgeti(index, i++)) {
		if(err = this->process(this->L->gettop())) {
			return err;
		}
		this->L->pop();
	}
	
	this->B->add(_FORMAT_ARRAY_HASH_SEP);
	
	while(this->L->next(index)) {
		if((this->L->type(-2) != LUA_TNUMBER) || (!is_int(x = this->L->tonumber(-2))) || (x >= i) || (x <= 0)) {
			if(err = this->process(this->L->gettop() - 1)) {
				return err;
			}
			if(err = this->process(this->L->gettop())) {
				return err;
			}
		}
		this->L->pop();
	}
				
	this->B->add(_FORMAT_TABLE_END);
	
	this->nesting--;
	return 0;
}

// adds value at index to buffer
// stack-balanced
// returns 0 or error code
int Saver::process(int index) {
	this->items++;
	if(this->items > this->maxitems) {
		return _SAVER_ERR_ITEMS;
	}
	if(!this->L->checkstack(2)) {
		return _SAVER_ERR_STACK;
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
						return this->process_number(index);
					}
				}
			}
			else {
				this->B->add(_FORMAT_NAN);
			}
			break;
		}
		case LUA_TSTRING: {
			return this->process_string(index);
			break;
		}
		case LUA_TTABLE: {
			this->L->pushvalue(index);
			this->L->rawget(_SAVER_I_REG);
			if(this->L->isnil(-1)) {
				this->L->pop();
				return this->process_table(index);
			}
			else {
				int err;
				this->B->add(_FORMAT_TABLE_REF);
				if (err = this->process_number(this->L->gettop())) {
					return err;
				}
				this->L->pop();
			}
			break;
		}
		case LUA_TFUNCTION: {
			return _SAVER_ERR_FUNCTION;
			break;
		}
		case LUA_TUSERDATA: {
			return _SAVER_ERR_USERDATA;
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			return _SAVER_ERR_USERDATA;
			break;
		}
		case LUA_TTHREAD: {
			return _SAVER_ERR_THREAD;
			break;
		}
		default: {
			return _SAVER_ERR_UNKNOWN;
			break;
		}
	}
	return 0;
}

void Saver::pushresult() {
	this->B->pushresult();
}
