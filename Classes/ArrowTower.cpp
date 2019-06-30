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

//�����ӵ�
Sprite* ArrowTower::ArrowTowerBullet()
{
	Sprite* bullet = Sprite::createWithSpriteFrameName("arrowBullet.png");
	//����λ�ú���ת�Ƕ�
	bullet->setPosition(rotateArrow->getPosition());
	bullet->setRotation(rotateArrow->getRotation());
	addChild(bullet);

	return bullet;
}

//��ת���ҹ���
void ArrowTower::rotateAndShoot(float dt)
{
	// 1�������ĵ���
	checkNearestEnemy();
	if (nearestEnemy != NULL)
	{
		//����˴ӷ�����ָ����޵�����
		//auto rotateVector = nearestEnemy->sprite->getPosition() - this->getPosition();
		auto rotateVector = nearestEnemy->getPosition() - this->getPosition();
		//getAngle����������rotateVector������X��֮��Ļ�����
		//�ýǵķ�����x��ָ��rotateVector������cocos2d��˳ʱ��Ϊ������˽���Ǹ���
		auto rotateRadians = rotateVector.getAngle();
		//��������ת���ɽǶ���
		auto rotateDegrees = CC_RADIANS_TO_DEGREES(-1 * rotateRadians);
		//speed��ʾ������ת���ٶȣ�0.5 / M_PI��ʵ���� 1 / 2PI������ʾ1������ת1��Բ��
		auto speed = 0.5 / M_PI;
		//������ת�ýǶ���Ҫ�ĳ���ʱ��
		auto rotateDuration = fabs(rotateRadians * speed);
		rotateArrow->runAction(Sequence::create(RotateTo::create(rotateDuration, rotateDegrees),
			CallFunc::create(CC_CALLBACK_0(ArrowTower::shoot, this)),
			NULL));
	}
}
void ArrowTower::shoot()
{   //������������ȡ�ӷ�����ָ������ĵ��˵�����shootVector��Ȼ���׼���ɵ�λ������
	//֮���趨�ӵ����Ĺ������볯��ֱ����ȥ��������δ�漰��ײ��⡣
	GameManager *instance = GameManager::getInstance();
	auto bulletVector = instance->bulletVector;
	//����е��˲��ҵ�������ֵ
	if (nearestEnemy != NULL && nearestEnemy->getCurrHp() > 0)
	{
		// 1 ���������ӵ������ӵ�����
		auto currBullet = ArrowTowerBullet();
		instance->bulletVector.pushBack(currBullet);
		// 2 �ƶ���ξ����ʱ�䣨�����Ϊ�ӵ��������ʣ�ͨ��getRate�����õ�
		auto moveDuration = getRate();
		// 3 �ӷ�����ָ��ֵ�����
		//Point shootVector = nearestEnemy->sprite->getPosition() - this->getPosition();
		Point shootVector = nearestEnemy->getPosition() - this->getPosition();
		// 4 ��������׼���ɵ�λ����
		shootVector.normalize();
		Point normalizedShootVector = -shootVector;
		// 5 ����붨�����Ļ�Ŀ�
		auto farthestDistance = Director::getInstance()->getWinSize().width;
		// 6 �������� = ��λ���� * ����ӵ�׷������
		Point overshotVector = normalizedShootVector * farthestDistance;
		Point offscreenPoint = (rotateArrow->getPosition() - overshotVector);
		// 7 �����ӵ�����
		currBullet->runAction(Sequence::create(MoveTo::create(moveDuration, offscreenPoint),
			CallFuncN::create(CC_CALLBACK_1(ArrowTower::removeBullet, this)),
			NULL));
		SimpleAudioEngine::getInstance()->playEffect("Sound/shoot.wav",false);
		currBullet = NULL;
	}
}

//���¼�������bullet�Ƴ�
void ArrowTower::removeBullet(Node* pSender)
{
	GameManager* instance = GameManager::getInstance();

	auto bulletVector = instance->bulletVector;

	Sprite* sprite = (Sprite*)pSender;
	instance->bulletVector.eraseObject(sprite);
	sprite->removeFromParentAndCleanup(true);
	sprite = NULL;
}
