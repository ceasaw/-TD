#pragma once

#ifndef _ATTACK_TOWER_H_
#define _ATTACK_TOWER_H_

#include "TowerBase.h"

class AttackTower :public TowerBase
{
public:

	virtual bool init();
	CREATE_FUNC(AttackTower);

	void shoot(float dt);
	void removeBullet(Node* pSender);
	Sprite* AttackTowerBullet();

private:
	Sprite* tower;

};
#endif // !_ATTACK_TOWER_H_
