#include "addrreg.h"

#include <stdlib.h> 

using namespace std;
#include <iostream>

#define LEFT 0
#define RIGHT 1

#define opposite(dir) (1 - dir)

AddrRegNode::AddrRegNode(intptr_t value, unsigned int id) {
	this->value = value;
	this->id = id;
	this->balance = 0;
	this->next[LEFT] = 0;
	this->next[RIGHT] = 0;
}

AddrReg::AddrReg() {
	this->count = 0;
	this->root = 0;
}

static AddrRegNode *rotate1(AddrRegNode *root, int dir) {
	AddrRegNode *save = root->next[opposite(dir)];
	root->next[opposite(dir)] = save->next[dir];
	save->next[dir] = root;
	return save;
}

static AddrRegNode *rotate2(AddrRegNode *root, int dir) {
	AddrRegNode *save = root->next[opposite(dir)]->next[dir];
	root->next[opposite(dir)]->next[dir] = save->next[opposite(dir)];
	save->next[opposite(dir)] = root->next[opposite(dir)];
	root->next[opposite(dir)] = save;
	save = root->next[opposite(dir)];
	root->next[opposite(dir)] = save->next[dir];
	save->next[dir] = root;
	return save;
}

static void adjust(AddrRegNode *root, int dir, signed char bal) {
	AddrRegNode *n1 = root->next[dir];
	AddrRegNode *n2 = n1->next[opposite(dir)];
	if(n1->balance == 0) {
		root->balance = n1->balance = 0;
	}
	else {
		if(n1->balance == bal) {
			root->balance = -bal;
			n1->balance = 0;
		}
		else {
			root->balance = 0;
			n1->balance = bal;
		}
	}
	n2->balance = 0;
}

static AddrRegNode *fix(AddrRegNode *root, int dir) {
	AddrRegNode *n = root->next[dir];
	signed char bal = (dir == LEFT) ? -1 : 1;
	if(n->balance == bal) {
		root->balance = n->balance = 0;
		root = rotate1(root, opposite(dir));
	}
	else {
		adjust(root, dir, bal);
		root = rotate2(root, opposite(dir));
	}
	return root;
}

unsigned int insert_(AddrRegNode *root, intptr_t value, unsigned int id) {
	if(root == 0) {
		root = new AddrRegNode(value, id);
		cout << "Creating new node with id " << id << endl;
		return 0;
	}
	AddrRegNode *it, *up[50];
	int upd[50], top = 0;
	it = root;
	for(;;) {
		if(it->value == value) {
			return it->id;
		}
		upd[top] = (it->value < value);
		up[top++] = it;
		if(it->next[upd[top - 1]] == 0) {
			break;
		}
		it = it->next[upd[top - 1]];
	}
	it->next[upd[top - 1]] = new AddrRegNode(value, id);
	while (--top >= 0) {
		up[top]->balance += (upd[top] == 0) ? -1 : 1;
		if(up[top]->balance == 0) {
			break;
		}
		else {
			if(abs(up[top]->balance) > 1) {
				up[top] = fix(up[top], upd[top]);
				if(top != 0) {
					up[top - 1]->next[upd[top - 1]] = up[top];
				}
				else {
					root = up[0];
				}
				break;
			}
		}
	}
	return 0;
}

unsigned int AddrReg::insert(intptr_t value) {
	if(this->root == 0) {
		root = new AddrRegNode(value, ++this->count);
		return 0;
	}
	AddrRegNode *it, *up[50];
	int upd[50], top = 0;
	it = this->root;
	for(;;) {
		if(it->value == value) {
			return it->id;
		}
		upd[top] = (it->value < value);
		up[top++] = it;
		if(it->next[upd[top - 1]] == 0) {
			break;
		}
		it = it->next[upd[top - 1]];
	}
	it->next[upd[top - 1]] = new AddrRegNode(value, ++this->count);
	while (--top >= 0) {
		up[top]->balance += (upd[top] == 0) ? -1 : 1;
		if(up[top]->balance == 0) {
			break;
		}
		else {
			if(abs(up[top]->balance) > 1) {
				up[top] = fix(up[top], upd[top]);
				if(top != 0) {
					up[top - 1]->next[upd[top - 1]] = up[top];
				}
				else {
					this->root = up[0];
				}
				break;
			}
		}
	}
	return 0;
}
