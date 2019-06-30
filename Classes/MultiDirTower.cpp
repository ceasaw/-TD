#include "MultiDirTower.h"
#include "GameManager.h"
#include "Enemy.h"
#include <algorithm>
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
bool MultiDirTower::init()
{
	if (!TowerBase::init())
	{
		return false;
	}

	setScope(130);
	setTowerValue(500);
	setLethality(4);
	setRate(8);
	tower = Sprite::createWithSpriteFrameName("multiDirTower.png");
	this->addChild(tower);

	this->schedule(schedule_selector(MultiDirTower::createBullet6), 2.0f);
	return true;
}


Sprite* MultiDirTower::MultiDirTowerBullet()
{
	Sprite* bullet = Sprite::createWithSpriteFrameName("bullet1.png");
	bullet->setPosition(0, tower->getContentSize().height / 4);
	this->addChild(bullet);

	return bullet;
}

void MultiDirTower::createBullet6(float dt)
{
	GameManager *instance = GameManager::getInstance();
	auto bulletVector = instance->bulletVector;
	int dirTotal = 8;
	this->checkNearestEnemy();
	if (nearestEnemy != NULL && nearestEnemy->getCurrHp() > 0)
	{
		for (int i = 0; i < dirTotal; i++)
		{
			auto currBullet = MultiDirTowerBullet();
			instance->bulletVector.pushBack(currBullet);

			auto moveDuration = getRate();

			Point shootVector;
			shootVector.x = 1;
			shootVector.y = tan(i * 2 * M_PI / dirTotal);
			Point normalizedShootVector;
			if (i >= dirTotal / 2)
			{
				shootVector.normalize();
				normalizedShootVector = shootVector;
			}
			else {
				shootVector.normalize();
				normalizedShootVector = -shootVector;
			}
			auto farthestDistance = Director::getInstance()->getWinSize().width;
			Point overshotVector = normalizedShootVector * farthestDistance;
			Point offscreenPoint = (currBullet->getPosition() - overshotVector);

			currBullet->runAction(Sequence::create(MoveTo::create(moveDuration, offscreenPoint),
				DelayTime::create(0.1f),
				CallFuncN::create(CC_CALLBACK_1(MultiDirTower::removeBullet, this)),
				NULL));
			SimpleAudioEngine::getInstance()->playEffect("Sound/shoot.wav");
			currBullet = NULL;
		}
	}
}


void MultiDirTower::removeBullet(Node* pSender)
{
	GameManager *instance = GameManager::getInstance();
	auto bulletVector = instance->bulletVector;

	Sprite *sprite = (Sprite *)pSender;
	instance->bulletVector.eraseObject(sprite);
	sprite->removeFromParent();
}