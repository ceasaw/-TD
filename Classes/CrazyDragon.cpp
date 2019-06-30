#include "CrazyDragon.h"
#include "SimpleAudioEngine.h"
#include "GameManager.h"
using namespace CocosDenshion;

bool CrazyDragon::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("crazyDragon.plist");

	//创建动画
	animationRight = createAnimation("CrazyDragonRight", 7, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationRight, "CrazyDragonRight");
	animationLeft = createAnimation("CrazyDragonLeft", 7, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationLeft, "CrazyDragonLeft");

	//初始化数据，此boss暂时不从文件读取
	//preDestination = Vec2(0, 0);
	//curDestination = Vec2(0, 0);
	setEnemyType(CRAZYDRAGON);
	setRunSpeed(15);
	setIsSpeedUp(false);
	setIsInitDestination(false);
	//创建血条
	createAndSetHpBar();
	schedule(schedule_selector(CrazyDragon::changeDirection), 0.75f);
	//schedule(schedule_selector(CrazyDragon::runFllowPoint), 0.1);
	return true;
}

void CrazyDragon::changeDirection(float dt)
{
	auto curr = currPoint();
	if (curr == NULL)
	{
		return;
	}
	if (curr->getPositionX() > this->getPosition().x)
	{
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("CrazyDragonRight")));
	}
	else
	{
		this->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("CrazyDragonLeft")));
	}
	//if (getIsSpeedUp() == true)
	//{
	//	SpeedUp();
	//	//setIsSpeedUp(false);
	//}
}

CrazyDragon* CrazyDragon::createCrazyDragon(Vector<Node*> points, int hp)
{
	CrazyDragon* pRet = new CrazyDragon();
	if (pRet != nullptr && pRet->init())
	{
		pRet->setPointsVector(points);
		pRet->setCurrHp(hp);
		pRet->setMaxHp(hp);
		pRet->runFllowPoint();

		pRet->autorelease();

		return pRet;
	}
	else
	{
		delete(pRet);
		pRet = NULL;
		return NULL;
	}
}

void CrazyDragon::enemyExpload()
{
	GameManager* instance = GameManager::getInstance();
	auto money = instance->getMoney();
	money += 500;
	instance->setMoney(money);
	//将血条设置为不可见
	hpBgSprite->setVisible(false);
	this->stopAllActions();
	unschedule(schedule_selector(CrazyDragon::changeDirection));
	this->runAction(FadeOut::create(3.0f));
	SimpleAudioEngine::getInstance()->playEffect("Sound/dead1.wav", false);

	
}

//void CrazyDragon::runFllowPoint(float dt)
//{
//	//解决如何刷新时间，实时设置moveto函数的问题
//	auto curPosition = this->getPosition();
//	//这一次用MoveBy实现 需要单位向量，需要两个目标点，preDestination和curDestination
//	
//	//初始化数据
//	if (isInitDestination == false)
//	{
//		auto point = currPoint();
//		preDestination = point->getPosition();
//
//		v1 = preDestination - curPosition;
//		setPosition(point->getPosition());
//		point = nextPoint();
//		curDestination = point->getPosition();
//		v2 = preDestination - curDestination;
//		setIsInitDestination(true);
//	}
//	//时刻刷新
//	v1 = preDestination - curPosition;
//	v2 = preDestination - curDestination;
//
//	if (v1.getLength() < v2.getLength())
//	{
//		Vec2 moveunit = v1;
//		moveunit.normalize();
//		runAction(MoveBy::create(getRunSpeed(), moveunit));
//	}
//	else  //刷新路径点
//	{
//		auto point = nextPoint();
//		preDestination = curDestination;
//		curDestination = point->getPosition();
//		v1 = preDestination - curPosition;
//		v2 = preDestination - curDestination;
//	}
//
//}

void CrazyDragon::createAndSetHpBar()
{
	//将没有大小的enemy重新设置图画的实际大小
	sprite = Sprite::createWithSpriteFrameName("CrazyDragonLeft_1.png");
	Size size = sprite->getContentSize();
	this->setContentSize(size);
	// 1 添加血条背景图片。
	hpBgSprite = Sprite::createWithSpriteFrameName("hpBg1.png");
	//hpBgSprite->setPosition(Point(this->getContentSize().width / 2+30, this->getContentSize().height+60));
	hpBgSprite->setPosition(Point(this->getContentSize().width / 2, this->getContentSize().height));
	hpBar = ProgressTimer::create(Sprite::createWithSpriteFrameName("hp1.png"));
	hpBar->setType(ProgressTimer::Type::BAR);
	hpBar->setMidpoint(Point(0, 0.5f));
	hpBar->setBarChangeRate(Point(1, 0));
	hpBar->setPercentage(hpPercentage);
	hpBar->setPosition(Point(hpBgSprite->getContentSize().width / 2, 
		hpBgSprite->getContentSize().height / 3 * 2));
	hpBgSprite->addChild(hpBar);
	hpBgSprite->setScaleX(2.0f);
	this->addChild(hpBgSprite);
}

//疯龙的技能是，当周围有小怪死亡的时候，会加速0.5s
void CrazyDragon::SpeedUp()
{
	setRunSpeed(1);
	this->runAction(Sequence::create(DelayTime::create(0.5f),
		CallFuncN::create([=](Node* pSender)
	{
		setRunSpeed(15);
	}), NULL));
	setIsSpeedUp(false);
}

