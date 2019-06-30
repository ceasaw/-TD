#include "GameManager.h"
#include "Enemy.h"
#include "TowerBase.h"
#include "GroupEnemy.h"

GameManager* GameManager::instance = NULL;

GameManager::GameManager() 
{

}
GameManager::~GameManager() {}

GameManager* GameManager::getInstance()
{
	if (instance == NULL)
		instance = new GameManager();
	return instance;
}
void GameManager::clear()
{
	enemyVector.clear();
	bulletVector.clear();
	//towerVector.clear();
	groupVector.clear();
	bossVector.clear();
}