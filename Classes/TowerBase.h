#pragma once
#ifndef _TOWER_BASE_H
#define _TOWER_BASE_H
#include "cocos2d.h"
USING_NS_CC;
class Enemy;

class TowerBase : public Sprite
{
public:
	TowerBase();
	virtual bool init();
	CREATE_FUNC(TowerBase);

	virtual void checkNearestEnemy();

	CC_SYNTHESIZE(int, scope, Scope);  // 塔的视线范围
	CC_SYNTHESIZE(int, lethality, Lethality);   // 杀伤力
	CC_SYNTHESIZE(int, towerValue, TowerValue);  //
	CC_SYNTHESIZE(float, rate, Rate);  //

protected:
	Enemy *nearestEnemy;    // 塔子视野内最近的敌人
};
#endif // !_TOWER_BASE_H

