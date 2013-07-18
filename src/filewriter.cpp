#include "filewriter.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

FileWriter::FileWriter(Lua *L, int index) : Writer(L, index) {}; // I should look up C++ OOP manual

FILE *FileWriter::tofile() {
	luaL_Stream *p = (luaL_Stream *) this->L->checkudata(this->index, LUA_FILEHANDLE);
	if(p->closef == NULL) {
		this->L->error("attempt to use a closed file");
	}
	return p->f;
}

void FileWriter::add(char c) {
	fwrite(&c, 1, 1, this->tofile());
}

void FileWriter::add(const char *s, size_t len) {
	fwrite(s, 1, len, this->tofile());
}

void FileWriter::addf(size_t maxsize, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(this->tofile(), fmt, args);
	va_end(args);
}

void FileWriter::pushresult() {
	this->L->pushboolean(1);
}
