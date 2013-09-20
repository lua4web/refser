#include "worker.hpp"

Worker::Worker(Lua *L) {
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
	
	L->getfield(_I_SELF, "doublecontext");
	this->doublecontext = L->toboolean(-1);
	L->pop();
	
	this->B = new FixBuf(L, L->gettop() + 1);
}

Worker::~Worker() {
	delete this->B;
}
