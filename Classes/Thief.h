#pragma once
#ifndef _THIEF_H_
#define _THIEF_H_

#include "Enemy.h"


class Thief : public Enemy
{
public:
	 bool init();
	 static Thief* Thief::createThief(Vector<Node*> points, int hp);
	 void changeDirection(float dt);
	 void enemyExpload();
};

#endif // !_THIEF_H_
