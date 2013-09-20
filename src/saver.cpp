#include "saver.hpp"

#include <stdlib.h>
#include "format.hpp"

void Saver::process_number(lua_Number x) {
	this->B->addf(_FORMAT_NUMBER_MAX, "%.*g", _FORMAT_NUMBER_LEN, x);
}

void Saver::process_string(const char *s, size_t len) {
	size_t i = 0;
	char esc;
	this->B->add(_FORMAT_STRING);
	while(i < len) {
		switch(s[i]) {
			case '\\': {
				esc = '\\';
				break;
			}
			case '\n': {
				esc = 'n';
				break;
			}
			case '\r': {
				esc = 'r';
				break;
			}
			case '"': {
				esc = '"';
				break;
			}
			case '\0': {
				esc = 'z';
				break;
			}
			default: {
				i++;
				continue;
			}
		}
		this->B->add(s, i);
		this->B->add('\\');
		this->B->add(esc);
		s += i + 1;
		len -= i + 1;
		i = 0;
	}
	this->B->add(s, len);
	this->B->add(_FORMAT_STRING);
}

void Saver::process_table(int index) {
	lua_Number x;
	int i = 1;
	
	this->nesting++;
	if(this->nesting > this->maxnesting) {
		throw _SAVER_ERR_TOODEEP;
	}
	
	this->count++;
	this->setid(index, this->count);
	
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
			this->L->gettable(_I_CONTEXT);
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

int Saver::pushresult() {
	this->L->pushnumber(this->count);
	this->L->setfield(_I_CONTEXT, "n");
	this->B->pushresult();
	return 1;
}
