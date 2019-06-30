#include "Bandit.h"
#include "GameManager.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

bool Bandit::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	this->setEnemyType(BANDIT);
	setIsDead(0);
	setRunSpeed(3.6);
	//sprite = Sprite::createWithSpriteFrameName("enemyRight3_1.png");
	//this->addChild(sprite);
	animationRight = createAnimation("enemyRight3", 4, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationRight, "runright3");
	animationLeft = createAnimation("enemyLeft3", 4, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationLeft, "runleft3");
	animationExplode = createAnimation("explode3", 6, 0.15f);
	AnimationCache::getInstance()->addAnimation(animationExplode, "explode3");

	createAndSetHpBar();

	schedule(schedule_selector(Bandit::changeDirection), 0.4f);
	return true;
}

Bandit* Bandit::createBandit(Vector<Node*> points, int hp)
{
	Bandit *pRet = new Bandit();
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

void Bandit::changeDirection(float dt)
{
	auto curr = currPoint();
	if (curr == NULL)
	{
		return;
	}
	if (curr->getPositionX() > this->getPosition().x)
	{
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runright3")));
	}
	else {
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runleft3")));
	}
}

void Bandit::enemyExpload()
{
	hpBgSprite->setVisible(false);
	this->stopAllActions();
	unschedule(schedule_selector(Bandit::changeDirection));
	// 修整爆炸动画的位置，因为它比其他状态都要大
	this->setAnchorPoint(Point(0.5f, 0.25f));
	this->runAction(Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("explode3"))
		, CallFuncN::create(CC_CALLBACK_0(Enemy::removeFromParent, this))
		, NULL));

}