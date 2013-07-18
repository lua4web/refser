#include "filewriter.hpp"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

FileWriter::FileWriter(Lua *L, int index) : Writer(L, index) {};

FILE *FileWriter::tofile() {
	luaL_Stream *p = (luaL_Stream *) this->L->checkudata(this->index, LUA_FILEHANDLE);
	if(p->closef == NULL) {
		this->L->error("attempt to use a closed file");
	}
	return p->f;
}

void FileWriter::add(char c) {
	if(!fwrite(&c, 1, 1, this->tofile())) {
		this->L->error("failed writing to file");
	}
}

void FileWriter::add(const char *s, size_t len) {
	if(fwrite(s, 1, len, this->tofile()) != len) {
		this->L->error("failed writing to file");
	}
}

void FileWriter::addf(size_t maxsize, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	if(vfprintf(this->tofile(), fmt, args) < 0) {
		this->L->error("failed writing to file");
	}
	va_end(args);
}

void FileWriter::pushresult() {
	this->L->pushboolean(1);
}
