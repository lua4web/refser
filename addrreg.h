#ifndef ADDRREG_H
#define ADDRREG_H

#include <stdint.h>

struct AddrRegNode {
	intptr_t value;
	unsigned int id;
	AddrRegNode *next[2];
	
	AddrRegNode(intptr_t  value, unsigned int id);
};

class AddrReg {
	private:
		unsigned int count;
		AddrRegNode *root;
	public:
		AddrReg();
		unsigned int insert(intptr_t value);
};

#endif
