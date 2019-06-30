#pragma once
#ifndef _MULRIDIR_TOWER_H_
#define _MULRIDIR_TOWER_H_
#include "TowerBase.h"


class MultiDirTower :public TowerBase
{
public:

	virtual bool init();
	CREATE_FUNC(MultiDirTower);


	//void shoot(float dt);
	void removeBullet(Node* pSender);
	Sprite* MultiDirTowerBullet();
	void createBullet6(float dt);
private:
	Sprite* tower;

};


#endif // !_MULRIDIR_TOWER_H_

