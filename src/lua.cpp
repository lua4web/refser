#include "lua.hpp"

Lua::Lua(lua_State *L) {
	this->L = L;
}

int Lua::error(const char *s) {
	return luaL_error(this->L, s);
}

void *Lua::newuserdata(size_t size) {
	return lua_newuserdata(this->L, size);
}

int Lua::gettop() {
	return lua_gettop(this->L);
}

void Lua::replace(int index) {
	lua_replace(this->L, index);
}

const char *Lua::tolstring(int index, size_t *len) {
	return lua_tolstring(this->L, index, len);
}

void Lua::pop(int n) {
	lua_pop(this->L, n);
}

void Lua::pop() {
	lua_pop(this->L, 1);
}

void Lua::pushlstring(const char *s, size_t len) {
	lua_pushlstring(this->L, s, len);
}

int Lua::rawgeti(int index, int n) {
	lua_rawgeti(this->L, index, n);
	return !lua_isnil(this->L, -1);
}

lua_Number Lua::tonumber(int index) {
	return lua_tonumber(this->L, index);
}

void Lua::pushnumber(lua_Number n) {
	lua_pushnumber(this->L, n);
}

void Lua::pushvalue(int index) {
	lua_pushvalue(this->L, index);
}

void Lua::rawset(int index) {
	lua_rawset(this->L, index);
}

int Lua::next(int index) {
	return lua_next(this->L, index);
}

int Lua::type(int index) {
	return lua_type(this->L, index);
}

int Lua::checkstack(int extra) {
	return lua_checkstack(this->L, extra);
}

int Lua::toboolean(int index) {
	return lua_toboolean(this->L, index);
}

int Lua::rawequal(int index1, int index2) {
	return lua_rawequal(this->L, index1, index2);
}

int Lua::rawget(int index) {
	lua_rawget(this->L, index);
	return !lua_isnil(this->L, -1);
}

int Lua::isnil(int index) {
	return lua_isnil(this->L, index);
}

void Lua::newtable() {
	lua_newtable(this->L);
}

void Lua::rawseti(int index, int n) {
	lua_rawseti(this->L, index, n);
}

void Lua::pushnil() {
	lua_pushnil(this->L);
}

void Lua::pushboolean(int b) {
	lua_pushboolean(this->L, b);
}

void Lua::settop(int index) {
	lua_settop(this->L, index);
}

void Lua::pushstring(const char *s) {
	lua_pushstring(this->L, s);
}

void Lua::remove(int index) {
	lua_remove(this->L, index);
}

const char *Lua::checklstring(int narg, size_t *l) {
	return luaL_checklstring(this->L, narg, l);
}

void *Lua::checkudata(int narg, const char *tname) {
	return luaL_checkudata(this->L, narg, tname);
}

void Lua::settable(int index) {
	lua_settable(this->L, index);
}

void Lua::settablei(int index, int n) {
	lua_pushnumber(this->L, n);
	lua_insert(this->L, -2);
	lua_settable(this->L, index);
}

int Lua::gettable(int index) {
	lua_gettable(this->L, index);
	return !lua_isnil(this->L, -1);
}

int Lua::gettablei(int index, int n) {
	lua_pushnumber(this->L, n);
	lua_gettable(this->L, index);
	return !lua_isnil(this->L, -1);
}

void Lua::getfield(int index, const char *field) {
	lua_getfield(this->L, index, field);
}
