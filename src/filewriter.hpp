#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "lua.hpp"
#include "writer.hpp"

class FileWriter: public Writer {
	private:
		FILE *tofile();
	public:
		FileWriter(Lua *L, int index);
		void add();
		void add(char c);
		void add(const char *s, size_t len);
		void add(const char *s);
		void addf(size_t maxsize, const char *fmt, ...);
		void addquoted(const char *s, size_t len);
		void pushresult();
};

#endif
