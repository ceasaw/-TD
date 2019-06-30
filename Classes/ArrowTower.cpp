#include "ArrowTower.h"
#include "GameManager.h"
#include "Enemy.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
bool ArrowTower::init()
{
	if (!TowerBase::init())
	{
		return false;
	}

	setScope(150);
	setLethality(2);
	setTowerValue(200);
	setRate(2);
	auto baseplate = Sprite::createWithSpriteFrameName("baseplate.png");
	addChild(baseplate);

	rotateArrow = Sprite::createWithSpriteFrameName("arrow.png");
	rotateArrow->setPosition(0, baseplate->getContentSize().height / 4);
	addChild(rotateArrow);

	schedule(schedule_selector(ArrowTower::rotateAndShoot), 0.3f);
	return true;
}

//创建子弹
Sprite* ArrowTower::ArrowTowerBullet()
{
	Sprite* bullet = Sprite::createWithSpriteFrameName("arrowBullet.png");
	//设置位置和旋转角度
	bullet->setPosition(rotateArrow->getPosition());
	bullet->setRotation(rotateArrow->getRotation());
	addChild(bullet);

	return bullet;
}

//旋转并且攻击
void ArrowTower::rotateAndShoot(float dt)
{
	// 1检测最近的敌人
	checkNearestEnemy();
	if (nearestEnemy != NULL)
	{
		//获得了从防御塔指向怪兽的向量
		//auto rotateVector = nearestEnemy->sprite->getPosition() - this->getPosition();
		auto rotateVector = nearestEnemy->getPosition() - this->getPosition();
		//getAngle方法将返回rotateVector向量与X轴之间的弧度数
		//该角的方向是x轴指向rotateVector向量，cocos2d中顺时针为正向，因此结果是负数
		auto rotateRadians = rotateVector.getAngle();
		//将弧度数转化成角度数
		auto rotateDegrees = CC_RADIANS_TO_DEGREES(-1 * rotateRadians);
		//speed表示炮塔旋转的速度，0.5 / M_PI其实就是 1 / 2PI，它表示1秒钟旋转1个圆。
		auto speed = 0.5 / M_PI;
		//计算旋转该角度需要的持续时间
		auto rotateDuration = fabs(rotateRadians * speed);
		rotateArrow->runAction(Sequence::create(RotateTo::create(rotateDuration, rotateDegrees),
			CallFunc::create(CC_CALLBACK_0(ArrowTower::shoot, this)),
			NULL));
	}
}
void ArrowTower::shoot()
{   //攻击方法：获取从防御塔指向最近的敌人的向量shootVector，然后标准化成单位向量。
	//之后设定子弹最大的攻击距离朝着直线射去。这里尚未涉及碰撞检测。
	GameManager *instance = GameManager::getInstance();
	auto bulletVector = instance->bulletVector;
	//最近有敌人并且敌人生命值
	if (nearestEnemy != NULL && nearestEnemy->getCurrHp() > 0)
	{
		// 1 将常见的子弹加入子弹数组
		auto currBullet = ArrowTowerBullet();
		instance->bulletVector.pushBack(currBullet);
		// 2 移动这段距离的时间（可理解为子弹发弹速率）通过getRate方法得到
		auto moveDuration = getRate();
		// 3 从防御塔指向怪的向量
		//Point shootVector = nearestEnemy->sprite->getPosition() - this->getPosition();
		Point shootVector = nearestEnemy->getPosition() - this->getPosition();
		// 4 将向量标准化成单位向量
		shootVector.normalize();
		Point normalizedShootVector = -shootVector;
		// 5 最长距离定义成屏幕的宽
		auto farthestDistance = Director::getInstance()->getWinSize().width;
		// 6 最大的向量 = 单位向量 * 最大子弹追击距离
		Point overshotVector = normalizedShootVector * farthestDistance;
		Point offscreenPoint = (rotateArrow->getPosition() - overshotVector);
		// 7 运行子弹动画
		currBullet->runAction(Sequence::create(MoveTo::create(moveDuration, offscreenPoint),
			CallFuncN::create(CC_CALLBACK_1(ArrowTower::removeBullet, this)),
			NULL));
		SimpleAudioEngine::getInstance()->playEffect("Sound/shoot.wav",false);
		currBullet = NULL;
	}
}

//将事件发起者bullet移除
void ArrowTower::removeBullet(Node* pSender)
{
	GameManager* instance = GameManager::getInstance();

	auto bulletVector = instance->bulletVector;

	Sprite* sprite = (Sprite*)pSender;
	instance->bulletVector.eraseObject(sprite);
	sprite->removeFromParentAndCleanup(true);
	sprite = NULL;
}
