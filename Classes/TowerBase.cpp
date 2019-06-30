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
	//gamemanager ����Ϸ������洢�˸������ݡ�
	// 1 ��ȡ��������
	GameManager* instance = GameManager::getInstance();
	auto enemyVector = instance->enemyVector;
	// 2 ��ʼ���������Ϊ��ǰ��������scope(�������)
	auto currMinDistant = 0;
	currMinDistant = this->scope;
	// 3 �����������������µ�ǰ��������������˵���ξ��룬����¼�¸õ���
	Enemy *enemyTemp = NULL;
	for (int i = 0; i < enemyVector.size(); i++)
	{
		auto enemy = enemyVector.at(i);
		//this->getPosition��һ�����Vec2
		//enenmy->sprite->getPosition �ڶ������Vec2
		Point v1 = this->getPosition();
		//auto sPrite = enemy->sprite;
		Point v2 = enemy->getPosition();
		double distance = (v1.getDistance(v2));
		//currMinDistant ������С����
		if (distance < currMinDistant) {
			currMinDistant = distance;
			enemyTemp = enemy;
		}
	}
	//�������еĵ��˺�õ������
	nearestEnemy = enemyTemp;
}