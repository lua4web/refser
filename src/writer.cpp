#include "writer.h"

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

void Writer::addquoted(const char *s, size_t len) {
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
