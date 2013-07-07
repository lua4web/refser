#include "fixbuf.h"

#include <string.h>

static char *fixbuf_prepare(fixbuf *B, size_t size) {
	if(B->size - B->used < size) { // not enough space
		size_t newsize = B->size * 2; // double buffer size
		if(newsize - B->used < size) { // not big enough?
			newsize = B->used + size;
		}
		if(newsize < B->used || newsize - B->used < size) {
			luaL_error(B->L, "fixbuf error: buffer is too large");
		}
		char *newbuff = lua_newuserdata(B->L, newsize);
		memcpy(newbuff, B->buff, B->used);
		if(lua_gettop(B->L) != B->index) {
			lua_replace(B->L, B->index);
		}
		B->buff = newbuff;
		B->size = newsize;
	}
	return &B->buff[B->used];
}

void fixbuf_init(lua_State *L, fixbuf *B, int index) {
	B->L = L;
	B->index = index;
	B->used = 0;
	B->size = FIXBUF_SIZE;
	B->buff = lua_newuserdata(L, FIXBUF_SIZE);
	if(lua_gettop(L) != index) {
		lua_replace(L, index);
	}
}

void fixbuf_addchar(fixbuf *B, char c) {
	char *buff = fixbuf_prepare(B, 1);
	memcpy(buff, &c, 1);
	B->used++;
}

void fixbuf_addlstring(fixbuf *B, const char *s, size_t len) {
	char *buff = fixbuf_prepare(B, len);
	memcpy(buff, s, len);
	B->used +=len;
}

void fixbuf_addqstring(fixbuf *B, const char *s, size_t len) {
	fixbuf_addchar(B, '"');
	size_t i = 0;
	char esc;
	while(i < len) {
		switch(s[i]) {
			case '\\': {
				esc = '\\';
				break;
			}
			case '\n': {
				esc = 'n';
				break;
			}
			case '\r': {
				esc = 'r';
				break;
			}
			case '"': {
				esc = '"';
				break;
			}
			case '\0': {
				esc = 'z';
				break;
			}
			default: {
				i++;
				continue;
			}
		}
		fixbuf_addlstring(B, s, i);
		fixbuf_addchar(B, '\\');
		fixbuf_addchar(B, esc);
		s += i + 1;
		len -= i + 1;
		i = 0;
	}
	fixbuf_addlstring(B, s, len);
	fixbuf_addchar(B, '"');
}

void fixbuf_addstring(fixbuf *B, const char *s) {
	fixbuf_addlstring(B, s, strlen(s));
}

void fixbuf_addvalue(fixbuf *B) {
	size_t len;
	const char *s = lua_tolstring(B->L, -1, &len);
	fixbuf_addlstring(B, s, len);
	lua_pop(B->L, 1);
}

void fixbuf_pushresult(fixbuf *B) {
	lua_pushlstring(B->L, B->buff, B->used);
}

void fixbuf_remove(fixbuf *B) {
	lua_remove(B->L, B->index);
}
