#include "fixbuf.h"

#include <string.h>
#include <stdarg.h>

#define FIXBUF_SIZE BUFSIZ

FixBuf::FixBuf(Lua *L, int index) : Writer(L, index) {
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

void FixBuf::addf(size_t maxsize, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	this->use(vsprintf(this->prepare(maxsize), fmt, args));
	va_end(args);
}

void FixBuf::pushresult() {
	this->L->pushlstring(this->buff, this->used);
}

void FixBuf::use(size_t size) {
	this->used += size;
}

void FixBuf::reset() {
	this->used = 0;
}
