#include "writer.hpp"

#include <string.h>

Writer::Writer(Lua *L, int index) {
	this->L = L;
	this->index = index;
}

void Writer::add() {
	size_t len;
	const char *s = this->L->tolstring(-1, &len);
	this->add(s, len);
	this->L->pop();
}

void Writer::add(const char *s) {
	this->add(s, strlen(s));
}
