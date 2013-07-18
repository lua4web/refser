#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "luapp.h"
#include "writer.h"

class FileWriter: public Writer {
	public:
		void add();
		void add(char c);
		void add(const char *s, size_t len);
		void add(const char *s);
		void addf(size_t maxsize, const char *fmt, ...);
		void addquoted(const char *s, size_t len);
		void pushresult();
};

#endif
