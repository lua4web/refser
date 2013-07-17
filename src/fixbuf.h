#ifndef FIXBUF_H
#define FIXBUF_H

#include "luapp.h"

#define FIXBUF_SIZE BUFSIZ

class FixBuf {
	private:
		char *buff;
		int index;
		size_t size;
		size_t used;
		Lua *L;
	public:
		FixBuf(Lua *L, int index);
		char *prepare(size_t size);
		void add();
		void add(char c);
		void add(const char *s, size_t len);
		void add(const char *s);
		void addquoted(const char *s, size_t len);
		void use(size_t size);
		void pushresult();
		void reset();
};

#endif
