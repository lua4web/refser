#ifndef LUAPP_H
#define LUAPP_H

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}

class Lua {
	private:
		lua_State *L;
	public:
		Lua(lua_State *L);
		
		int error(const char *s);
		void *newuserdata(size_t size);
		int gettop();
		void replace(int index);
		void insert(int index);
		const char *tolstring(int index, size_t *len);
		void pop(int n);
		void pop();
		void pushlstring(const char *s, size_t len);
		int rawgeti(int index, int n);
		lua_Number tonumber(int index);
		void pushnumber(lua_Number n);
		void pushvalue(int index);
		void rawset(int index);
		int next(int index);
		int type(int index);
		int checkstack(int extra);
		int toboolean(int index);
		int rawequal(int index1, int index2);
		int rawget(int index);
		int isnil(int index);
		void newtable();
		void rawseti(int index, int n);
		void pushnil();
		void pushboolean(int b);
		void settop(int index);
		void pushstring(const char *s);
		void remove(int index);
		const char *checklstring(int narg, size_t *l);
		void *checkudata(int narg, const char *tname);
		void settable(int index);
		void settablei(int index, int n);
		int gettable(int index);
		int gettablei(int index, int n);
		void getfield(int index, const char *field);
		void setfield(int index, const char *field);
};

#endif
