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

	//��������
	animationRight = createAnimation("CrazyDragonRight", 7, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationRight, "CrazyDragonRight");
	animationLeft = createAnimation("CrazyDragonLeft", 7, 0.1f);
	AnimationCache::getInstance()->addAnimation(animationLeft, "CrazyDragonLeft");

	//��ʼ�����ݣ���boss��ʱ�����ļ���ȡ
	//preDestination = Vec2(0, 0);
	//curDestination = Vec2(0, 0);
	setEnemyType(CRAZYDRAGON);
	setRunSpeed(15);
	setIsSpeedUp(false);
	setIsInitDestination(false);
	//����Ѫ��
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
	//��Ѫ������Ϊ���ɼ�
	hpBgSprite->setVisible(false);
	this->stopAllActions();
	unschedule(schedule_selector(CrazyDragon::changeDirection));
	this->runAction(FadeOut::create(3.0f));
	SimpleAudioEngine::getInstance()->playEffect("Sound/dead1.wav", false);

	
}

//void CrazyDragon::runFllowPoint(float dt)
//{
//	//������ˢ��ʱ�䣬ʵʱ����moveto����������
//	auto curPosition = this->getPosition();
//	//��һ����MoveByʵ�� ��Ҫ��λ��������Ҫ����Ŀ��㣬preDestination��curDestination
//	
//	//��ʼ������
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
//	//ʱ��ˢ��
//	v1 = preDestination - curPosition;
//	v2 = preDestination - curDestination;
//
//	if (v1.getLength() < v2.getLength())
//	{
//		Vec2 moveunit = v1;
//		moveunit.normalize();
//		runAction(MoveBy::create(getRunSpeed(), moveunit));
//	}
//	else  //ˢ��·����
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
	//��û�д�С��enemy��������ͼ����ʵ�ʴ�С
	sprite = Sprite::createWithSpriteFrameName("CrazyDragonLeft_1.png");
	Size size = sprite->getContentSize();
	this->setContentSize(size);
	// 1 ���Ѫ������ͼƬ��
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

//�����ļ����ǣ�����Χ��С��������ʱ�򣬻����0.5s
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

