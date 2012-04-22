#include "red.h"
red::red()
{
	name = "Red";
	delete head;
	move * temp;
	moveTrie * t;
	head = new moveTrie;
	head->insert(new move("Red/A"));
	head->insert(new move("Red/C"));
	head->insert(new special("Red/-D"));

	delete neutral;
	neutral = new utility("Red/NS");
	head->insert(neutral);

	head->insert(6, new utility("Red/W"));
	head->insert(4, new utility("Red/WQ"));
	temp = new utility("Red/wsh");

	t = head->insert(6, temp);
	t = t->insert(5);
	t = t->insert(6);
	t->insert(temp);
	
	temp = new utility("Red/qsh");
	t = head->insert(4);
	t = t->insert(5);
	t = t->insert(4);
	t->insert(temp);

	temp = new move("Red/JQ");
	temp->activation = 0;
	head->insert(7, temp);

	temp = new move("Red/JF");
	temp->activation = 0;
	head->insert(9, temp);

	temp = new move("Red/JN");
	temp->activation = 0;
	head->insert(8, temp);

	cMove = neutral;
}
