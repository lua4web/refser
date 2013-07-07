#ifndef FIXBUF_H
#define FIXBUF_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define FIXBUF_SIZE BUFSIZ

typedef struct fixbuf {
	char *buff;
	int index;
	size_t size;
	size_t used;
	lua_State *L;
} fixbuf;

void fixbuf_init(lua_State *L, fixbuf *B, int index);
void fixbuf_addchar(fixbuf *B, char c);
void fixbuf_addlstring(fixbuf *B, const char *s, size_t len);
void fixbuf_addstring(fixbuf *B, const char *s);
void fixbuf_addvalue(fixbuf *B);
void fixbuf_pushresult(fixbuf *B);
void fixbuf_remove(fixbuf *B);

#endif
