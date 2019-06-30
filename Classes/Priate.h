#pragma once
#ifndef _PRIATE_H
#define _PRIATE_H
#include "Enemy.h"

class Pirate : public Enemy
{
public:
	 bool init();
	static Pirate* createPirate(Vector<Node*> points, int hp);
	void changeDirection(float dt);
	void enemyExpload();
};


#endif // !_PRIATE_H
