#include "fixbuf.h"

#include <string.h>

FixBuf::FixBuf(Lua *L, int index) {
	this->L = L;
	this->index = index;
	this->used = 0;
	this->size = FIXBUF_SIZE;
	this->buff = (char *) L->newuserdata(FIXBUF_SIZE);
	if(L->gettop() != index) {
		L->replace(index);
	}
}

char *FixBuf::prepare(size_t size) {
	if(this->size - this->used < size) {
		char *newbuff;
		size_t newsize = this->size * 2;
		if(newsize - this->used < size) {
			newsize = this->used + size;
		}
		if(newsize < this->used || newsize - this->used < size) {
			this->L->error("fixbuf error: buffer is too large");
		}
		newbuff = (char *) this->L->newuserdata(newsize);
		memcpy(newbuff, this->buff, this->used);
		if(this->L->gettop() != this->index) {
			this->L->replace(this->index);
		}
		this->buff = newbuff;
		this->size = newsize;
	}
	return &this->buff[this->used];
}

void FixBuf::add(char c) {
	*this->prepare(1) = c;
	this->use(1);
}

void FixBuf::add(const char *s, size_t len) {
	memcpy(this->prepare(len), s, len);
	this->use(len);
}

void FixBuf::add(const char *s) {
	this->add(s, strlen(s));
}

void FixBuf::add() {
	size_t len;
	const char *s = this->L->tolstring(-1, &len);
	this->add(s, len);
	this->L->pop();
}

void FixBuf::addquoted(const char *s, size_t len) {
	size_t i = 0;
	char esc;
	this->add('"');
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
		this->add(s, i);
		this->add('\\');
		this->add(esc);
		s += i + 1;
		len -= i + 1;
		i = 0;
	}
	this->add(s, len);
	this->add('"');
}

void FixBuf::use(size_t size) {
	this->used += size;
}

void FixBuf::pushresult() {
	this->L->pushlstring(this->buff, this->used);
}

void FixBuf::reset() {
	this->used = 0;
}
