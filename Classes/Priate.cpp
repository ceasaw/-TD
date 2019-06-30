#include "Priate.h"
#include "GameManager.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
bool Pirate::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	this->setEnemyType(PRIATE);
	this->setIsDead(0);
	setRunSpeed(3.3);
	//sprite = Sprite::createWithSpriteFrameName("enemyRight2_1.png");
	//this->addChild(sprite,-10);
	animationRight = createAnimation("enemyRight2", 4, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationRight, "runright2");
	animationLeft = createAnimation("enemyLeft2", 4, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationLeft, "runleft2");
	animationExplode = createAnimation("explode2", 6, 0.15f);
	AnimationCache::getInstance()->addAnimation(animationExplode, "explode2");

	createAndSetHpBar();
	schedule(schedule_selector(Pirate::changeDirection), 0.4f);
	return true;
}

Pirate* Pirate::createPirate(Vector<Node*> points, int hp)
{
	Pirate *pRet = new Pirate();
	if (pRet && pRet->init())
	{
		pRet->setPointsVector(points);
		pRet->setMaxHp(hp);
		pRet->setCurrHp(hp);
		pRet->runFllowPoint();

		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void Pirate::changeDirection(float dt)
{
	//Point P = this->getPosition();
	//sprite->setPosition(P);
	auto curr = currPoint();
	if (curr == NULL)
	{
		return;
	}
	if (curr->getPositionX() > this->getPosition().x)
	{
		//float X = sprite->getPosition().x;
		//if (X > 50.0)
		//{
		//	CCLOG("OK");
		//}
		
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runright2")));
	}
	else {
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runleft2")));
	}
}

void Pirate::enemyExpload()
{
	hpBgSprite->setVisible(false);
	this->stopAllActions();
	unschedule(schedule_selector(Pirate::changeDirection));
	// 修整爆炸动画的位置，因为它比其他状态都要大
	this->setAnchorPoint(Point(0.5f, 0.25f));
	this->runAction(Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("explode2"))
		, CallFuncN::create(CC_CALLBACK_0(Enemy::removeFromParent, this))
		, NULL));


}
