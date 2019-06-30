#include "TowerBase.h"
#include "GameManager.h"
#include "Enemy.h"
TowerBase::TowerBase()
	:scope(0)
	, towerValue(0)
	, lethality(0)
	, rate(0)
	, nearestEnemy(NULL)
{
}

bool TowerBase::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	scheduleUpdate();
	return true;
}


void TowerBase::checkNearestEnemy()
{
	//gamemanager 是游戏的主类存储了各种数据。
	// 1 获取敌人数组
	GameManager* instance = GameManager::getInstance();
	auto enemyVector = instance->enemyVector;
	// 2 初始化射击距离为当前的炮塔的scope(射击距离)
	auto currMinDistant = 0;
	currMinDistant = this->scope;
	// 3 遍历敌人向量，更新当前距离炮塔最近敌人的这段距离，并记录下该敌人
	Enemy *enemyTemp = NULL;
	for (int i = 0; i < enemyVector.size(); i++)
	{
		auto enemy = enemyVector.at(i);
		//this->getPosition第一个点的Vec2
		//enenmy->sprite->getPosition 第二个点的Vec2
		Point v1 = this->getPosition();
		//auto sPrite = enemy->sprite;
		Point v2 = enemy->getPosition();
		double distance = (v1.getDistance(v2));
		//currMinDistant 代表最小距离
		if (distance < currMinDistant) {
			currMinDistant = distance;
			enemyTemp = enemy;
		}
	}
	//遍历所有的敌人后得到最近的
	nearestEnemy = enemyTemp;
}