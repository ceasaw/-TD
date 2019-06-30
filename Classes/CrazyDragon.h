#pragma once
#ifndef _CRAZY_DRAGON_H_
#define _CRAZY_DRAGON_H_

#include "Enemy.h"

class CrazyDragon : public Enemy
{
public:
	bool init();
	static CrazyDragon* createCrazyDragon(Vector<Node*> points, int hp);
	void changeDirection(float dt);

	void enemyExpload();
	//void runFllowPoint(float dt) ;
	void createAndSetHpBar();
	virtual void SpeedUp();
	CC_SYNTHESIZE(bool, isSpeedUp, IsSpeedUp);
	CC_SYNTHESIZE(bool, isInitDestination, IsInitDestination);
	//Vec2 preDestination;
	//Vec2 curDestination;
	//Vec2 v1;//preDestination到当前位置的向量
	//Vec2 v2;//preDestination到curDestination的向量
};

#endif // !_CRAZY_DRAGON_H_

