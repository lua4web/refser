#ifndef WRITER_H
#define WRITER_H

#include "lua.hpp"

class Writer {
	protected:
		Lua *L;
		int index;
	public:
		Writer(Lua *L, int index);
		void add();
		virtual void add(char c) = 0;
		virtual void add(const char *s, size_t len) = 0;
		void add(const char *s);
		virtual void addf(size_t maxsize, const char *fmt, ...) = 0;
		virtual void pushresult() = 0;
};

#endif
