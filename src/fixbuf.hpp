#ifndef FIXBUF_H
#define FIXBUF_H

#include "lua.hpp"
#include "writer.hpp"

class FixBuf: public Writer {
	private:
		char *buff;
		size_t size;
		size_t used;
		
		char *prepare(size_t size);
		void use(size_t size);
	public:
		FixBuf(Lua *L, int index);
		void add();
		void add(char c);
		void add(const char *s, size_t len);
		void add(const char *s);
		void addf(size_t maxsize, const char *fmt, ...);
		void addquoted(const char *s, size_t len);
		void pushresult();
		void reset();
};

#endif
