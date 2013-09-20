#include "loader.hpp"

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "format.hpp"


#define ensure(cond) { \
	if(!(cond)) { \
		throw _LOADER_ERR_MAILFORMED; \
	} \
}

#define valid_number_char(c) (((c) <= '9' && (c) >= '0') || (c) == 'e' || (c) == '.' || (c) == '-')

void Loader::eat() {
	this->s++;
	this->len--;
}

void Loader::eat(size_t size) {
	this->s += size;
	this->len -= size;
}

void Loader::process_number() {
	size_t i = 0;
	lua_Number x;
	while(valid_number_char(this->s[i]) && i < _FORMAT_NUMBER_MAX) {
		i++;
	}
	ensure(!valid_number_char(this->s[i]));
	x = strtod(this->s, NULL);
	ensure(x || (i == 1 && *this->s == '0'));
	this->eat(i);
	ensure(!isnan(x));
	this->L->pushnumber(x);
}

void Loader::process_string() {
	char esc;
	size_t i = 0;
	this->B->reset();
	while(this->s[i] != '"') {
		if(this->s[i] == '\\') {
			ensure(this->len > i + 1);
			switch(this->s[i+1]) {
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
					throw _LOADER_ERR_MAILFORMED;
					break;
				}
			}
			this->B->add(this->s, i);
			this->B->add(esc);
			this->eat(i + 2);
			i = 0;
		}
		else {
			i++;
		}
		ensure(this->len > i);
	}
	
	this->B->add(this->s, i);
	
	this->eat(i + 1);
	
	this->B->pushresult();		
}

void Loader::process_table() {
	int i = 1;
	
	this->nesting++;
	if(this->nesting > this->maxnesting) {
		throw _LOADER_ERR_TOODEEP;
	}
	
	while(*this->s != _FORMAT_ARRAY_HASH_SEP) {
		this->process(_LOADER_ROLE_VALUE);
		this->L->rawseti(-2, i++);
	}
	
	this->eat();
	
	while(*this->s != _FORMAT_TABLE_END) {
		this->process(_LOADER_ROLE_KEY);
		this->process(_LOADER_ROLE_VALUE);
		this->L->rawset(-3);
	}
	
	this->eat();
	this->nesting--;
}

// reads next value from string
// puts it on top of lua stack
void Loader::process(int role) {
	this->items++;
	if(this->items > this->maxitems) {
		throw _LOADER_ERR_ITEMS;
	}
	ensure(this->len);
	if(!this->L->checkstack(2)) {
		throw _LOADER_ERR_STACK;
	}
	this->eat();
	switch(this->s[-1]) {
		case _FORMAT_NIL: {
			ensure(role == _LOADER_ROLE_NONE);
			this->L->pushnil();
			break;
		}
		case _FORMAT_TRUE: {
			this->L->pushboolean(1);
			break;
		}
		case _FORMAT_FALSE: {
			this->L->pushboolean(0);
			break;
		}
		case _FORMAT_INF: {
			this->L->pushnumber(HUGE_VAL);
			break;
		}
		case _FORMAT_MINF: {
			this->L->pushnumber(HUGE_VAL * -1);
			break;
		}
		case _FORMAT_NAN: {
			ensure(role != _LOADER_ROLE_KEY);
			this->L->pushnumber(NAN);
			break;
		}
		case _FORMAT_TABLE_REF: {
			this->process_number();
			this->L->gettable(_I_CONTEXT);
			if(this->L->isnil(-1)) {
				throw _LOADER_ERR_CONTEXT;
			}
			break;
		}
		case _FORMAT_TABLE_EXPLICIT: {
			int x;
			this->process_number();
			x = this->L->tonumber(-1);
			this->L->pop();
			
			this->count = (this->count > x) ? this->count : x;
			this->L->newtable();
			this->setid(this->L->gettop(), x);
			this->process_table();
			break;
		}
		case _FORMAT_TABLE_START: {
			this->count++;
			this->L->newtable();
			this->setid(this->L->gettop(), this->count);
			this->process_table();
			break;
		}
		case _FORMAT_NUMBER: {
			this->process_number();
			break;
		}
		case _FORMAT_STRING: {
			this->process_string();
			break;
		}
		default: {
			throw _LOADER_ERR_MAILFORMED;
			break;
		}
	}
}

int Loader::done() {
	return !this->len;
}

int Loader::pushresult() {
	Worker::pushresult();
	this->L->pushnumber(this->L->gettop() - _I_X - 1);
	this->L->replace(_I_X + 1);
	return this->L->gettop() - _I_X;
}
