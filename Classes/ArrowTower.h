#pragma once
#ifndef _ARROW_TOWER_H_
#define _ARROW_TOWER_H_
#include "Towerbase.h"

class ArrowTower :public TowerBase
{
public:

	virtual bool init();
	CREATE_FUNC(ArrowTower);

	void rotateAndShoot(float dt);
	void shoot();
	void removeBullet(Node* pSender);
	Sprite* ArrowTowerBullet();

private:
	Sprite* rotateArrow;

};





#endif // !_ARROW_TOWER_H_

