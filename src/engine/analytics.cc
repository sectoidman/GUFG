#include "analytics.h"
frame::frame()
{
	next = NULL;
}

void frame::cull()
{
	if(next){ 
		next->cull(); 
		delete next;
	}
	next = NULL;
}

sequence::sequence()
{
	start = NULL;
	curr = NULL;
}

sequence::~sequence()
{
	head->cull();
	delete start;
	head = NULL;
	curr = NULL;
}

sequence::append(frame * n)
{
	if(!start) start = n;
	else curr->next = n;
	curr = n;
}
