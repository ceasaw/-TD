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

	CC_SYNTHESIZE(int, scope, Scope);  // �������߷�Χ
	CC_SYNTHESIZE(int, lethality, Lethality);   // ɱ����
	CC_SYNTHESIZE(int, towerValue, TowerValue);  //
	CC_SYNTHESIZE(float, rate, Rate);  //

protected:
	Enemy *nearestEnemy;    // ������Ұ������ĵ���
};
#endif // !_TOWER_BASE_H

