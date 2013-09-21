#ifndef SAVER_H
#define SAVER_H

#include "lua.hpp"
#include "worker.hpp"

#define _SAVER_ERR_TOODEEP "refser.save error: table is too deep"
#define _SAVER_ERR_STACK "refser.save error: lua stack exhausted"
#define _SAVER_ERR_FUNCTION "refser.save error: attempt to save function"
#define _SAVER_ERR_USERDATA "refser.save error: attempt to save userdata"
#define _SAVER_ERR_THREAD "refser.save error: attempt to save thread"
#define _SAVER_ERR_ITEMS "refser.save error: too many items"
#define _SAVER_ERR_TOOLONG "refser.save error: tuple is too long"
#define _SAVER_ERR_MODE "refser.save error: mode prohibits saving"

class Saver: public Worker {
	private:
		void process_number(lua_Number x);
		void process_string(const char *s, size_t len);
		void process_table(int index, int id);
	public:
		explicit Saver(Lua *L) : Worker(L) { };
		void process(int index);
		int pushresult();
};

#endif
