#include "addrreg.h"

#define LEFT 0
#define RIGHT 1

AddrRegNode::AddrRegNode(intptr_t value, unsigned int id) {
	this->value = value;
	this->id = id;
	this->next[LEFT] = 0;
	this->next[RIGHT] = 0;
}

AddrReg::AddrReg() {
	this->count = 0;
	this->root = 0;
}

unsigned int AddrReg::insert(intptr_t target) {
	return 0;
}
