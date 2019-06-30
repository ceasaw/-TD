#pragma once
#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

#include "cocos2d.h"
class GroupEnemy;
class TowerBase;
class Enemy;
USING_NS_CC;

class GameManager
{
public:
	GameManager();
	~GameManager();
	Vector<Enemy*> enemyVector;
	Vector<Sprite*> bulletVector;
	Vector<TowerBase*> towerVector;
	Vector<GroupEnemy*> groupVector;
	Vector<Enemy*> bossVector;
	static GameManager* getInstance();
	void clear();

	CC_SYNTHESIZE(int, money, Money);
	CC_SYNTHESIZE(int, groupNum, GroupNum);
	CC_SYNTHESIZE(std::string, curMapName, CurMapName);
	CC_SYNTHESIZE(std::string, currLevelFile, CurrLevelFile);
	CC_SYNTHESIZE(std::string, nextLevelFile, NextLevelFile);
	CC_SYNTHESIZE(bool, isFinishedAddGroup, IsFinishedAddGroup);
	CC_SYNTHESIZE(std::string, curBgName, CurBgName);

private:
	static GameManager * instance;
};


#endif // !_GAME_MANAGER_H

