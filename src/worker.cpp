#include "worker.hpp"

#include <string.h>

Worker::Worker(Lua *L) {
	const char *mode;
	this->L = L;
	this->nesting = 0;
	this->items = 0;
	
	L->getfield(_I_SELF, "context");
	L->insert(_I_CONTEXT);
	
	L->getfield(_I_CONTEXT, "n");
	this->count = L->tonumber(-1);
	L->pop();
	
	L->getfield(_I_SELF, "maxnesting");
	this->maxnesting = L->tonumber(-1);
	L->pop();
	
	L->getfield(_I_SELF, "maxtuple");
	this->maxtuple = L->tonumber(-1);
	L->pop();
	
	L->getfield(_I_SELF, "maxitems");
	this->maxitems = L->tonumber(-1);
	L->pop();
	
	L->getfield(_I_SELF, "mode");
	this->mode = L->tolstring(-1, NULL);
	if(strchr(this->mode, 's')) {
		this->savemode = 1;
	}
	else {
		this->savemode = 0;
	}
	if(strchr(this->mode, 'l')) {
		this->loadmode = 1;
	}
	else {
		this->loadmode = 0;
	}
	L->pop();
	
	this->B = new FixBuf(L, L->gettop() + 1);
}

Worker::~Worker() {
	delete this->B;
}

void Worker::setid(int index, int id) {
	if(this->savemode) {
		this->L->pushvalue(index);
		this->L->pushnumber(id);
		this->L->settable(_I_CONTEXT);
	}
	
	if(this->loadmode) {
		this->L->pushvalue(index);
		this->L->settablei(_I_CONTEXT, id);
	}
}

int Worker::pushresult() {
	this->L->pushnumber(this->count);
	this->L->setfield(_I_CONTEXT, "n");
	return 0;
}
