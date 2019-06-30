#include "TowerPanelLayer.h"

bool TowerPanelLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	setChooseTowerType(TowerType::ANOTHER);

	auto sprite = Sprite::createWithSpriteFrameName("towerPos.png");
	sprite->setPosition(Point(0, 0));
	this->addChild(sprite);

	//��������ֱ��ʾ����ѡ��
	sprite1 = Sprite::createWithSpriteFrameName("ArrowTower1.png");
	sprite1->setAnchorPoint(Point(0.5f, 0));
	sprite1->setPosition(Point(-sprite->getContentSize().width, sprite->getContentSize().height / 2));
	this->addChild(sprite1);

	sprite2 = Sprite::createWithSpriteFrameName("AttackTower1.png");
	sprite2->setAnchorPoint(Point(0.5f, 0));
	sprite2->setPosition(Point(0, sprite->getContentSize().height / 2));
	this->addChild(sprite2);

	sprite3 = Sprite::createWithSpriteFrameName("MultiDirTower1.png");
	sprite3->setAnchorPoint(Point(0.5f, 0));
	sprite3->setPosition(Point(sprite->getContentSize().width, sprite->getContentSize().height / 2));
	this->addChild(sprite3);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(TowerPanelLayer::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(TowerPanelLayer::onTouchEnded, this);
	//Ϊÿ��ѡ��󶨴����¼�
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, sprite1);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener->clone(), sprite2);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener->clone(), sprite3);
	return true;
}
//��                            //��
//���ȴ�����Ļ��ʱ�򣬻�ȡ����openGL������ �������ȵ�ת���� cocos2d �µ����� ��
//Ȼ���ȡ������Node�Ĵ�С���ٽ����껻���Node����ϵ�������
//ʵ���ж��Ƿ�������Node��
//��Node�������м�����������������ת����
//convertToNodeSpace�������԰���������ת������ǰnode�ı�������ϵ��
//convertToWorldSpace�������԰ѻ��ڵ�ǰnode�ı�������ϵ�µ�����ת������������ϵ�У�
//convertTouchToNodeSpace����������԰���Ļ����ϵת����GL����ϵ����ת�������ڵ�ı��������¡�
bool TowerPanelLayer::onTouchBegan(Touch *touch, Event *event)
{
	// 1 ���ش����¼���ǰ���õ�Ŀ��ڵ� 
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	// 2 ��touch�����б������Ļ����ϵת����GL����ϵ����ת����Ŀ��ڵ�ı���������
	Point locationInNode = target->convertTouchToNodeSpace(touch);
	// 3 ����Ŀ��ڵ�ľ�������
	Size size = target->getContentSize();
	Rect rect = Rect(0, 0, size.width, size.height);
	// 4 ͨ��containsPoint�������жϴ������ڲ���Ŀ��ڵ�ľ��������ڣ����Ƿ�ѡ��
	if (rect.containsPoint(locationInNode))
	{
		//����ĳѡ��ʱ������������͸���ȣ�����ұ��������Ǳ�ѡ�е�
		target->setOpacity(180);
		return true;
	}
	return false;
}


void TowerPanelLayer::onTouchEnded(Touch* touch, Event* event)
{
	// ����ѡ���Ŀ��ȷ������������ 
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	if (target == sprite1)
	{
		//����ѡ��ķ�����������
		chooseTowerType = ARROW_TOWER;
		//��ԭѡ�еķ�������͸����
		target->setOpacity(255);
	}
	else if (target == sprite2)
	{
		chooseTowerType = ATTACK_TOWER;
		target->setOpacity(255);
	}
	else if (target == sprite3)
	{
		chooseTowerType = MULTIDIR_TOWER;
		target->setOpacity(255);
	}
	else {
		chooseTowerType = ANOTHER;
	}
}

