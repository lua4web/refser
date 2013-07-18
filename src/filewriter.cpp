#include "filewriter.h"

#include <string.h>
#include <stdarg.h>

void FileWriter::add(char c) {
	
}

void FileWriter::add(const char *s, size_t len) {
	
}

void FileWriter::addf(size_t maxsize, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	
	va_end(args);
}

void FileWriter::pushresult() {
	this->L->pushboolean(1);
}
