#include "Thief.h"
#include "GameManager.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
bool Thief::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	this->setEnemyType(THIEF);
	this->setIsDead(0);
	setRunSpeed(2.8);
	//sprite = Sprite::createWithSpriteFrameName("enemyRight1_1.png");
	//this->addChild(sprite);
	animationRight = createAnimation("enemyRight1", 4, 0.1f);
	
	// name - animation : 是一组 键-值对(key-value) 的关系。
	//void addAnimation(Animation *animation, const std::string& name);
	//向动画缓存中添加了一个key是"runright"的动画。
	AnimationCache::getInstance()->addAnimation(animationRight, "runright1");
	animationLeft = createAnimation("enemyLeft1", 4, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationLeft, "runleft1");
	//animationExplode = createAnimation("explode2", 6, 0.15f);
	//AnimationCache::getInstance()->addAnimation(animationExplode, "explode2");
	animationExplode = createAnimation("explode1", 6, 0.15f);
	AnimationCache::getInstance()->addAnimation(animationExplode, "explode1");

	createAndSetHpBar();
	schedule(schedule_selector(Thief::changeDirection), 0.4f);
	return true;
}


Thief* Thief::createThief(Vector<Node*> points, int hp)
{
	Thief *pRet = new Thief();
	//如果已经创建并且初始化
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

void Thief::changeDirection(float dt)
{
	auto curr = currPoint();
	if (curr == NULL)
	{
		return;
	}
	if (curr->getPositionX() > this->getPosition().x)
	{
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runright1")));
	}
	else {
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runleft1")));
	}
}

void Thief::enemyExpload()
{

	hpBgSprite->setVisible(false);
	this->stopAllActions();
	unschedule(schedule_selector(Thief::changeDirection));
	// 修整爆炸动画的位置，因为它比其他状态都要大
	this->setAnchorPoint(Point(0.5f, 0.25f));
	this->runAction(Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("explode1"))
		, CallFuncN::create(CC_CALLBACK_0(Enemy::removeFromParent, this))
		, NULL));
}
