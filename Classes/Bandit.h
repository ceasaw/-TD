#pragma once
#ifndef _BANDIT_H_
#define _BANDIT_H_
#include "Enemy.h"

class Bandit : public Enemy
{
public:
	 bool init();
	static Bandit* createBandit(Vector<Node*> points, int hp);
	void changeDirection(float dt);
	void enemyExpload();
};
#endif // !_BANDIT_H_


