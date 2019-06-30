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

	//三个精灵分别表示三个选项
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
	//为每个选项绑定触摸事件
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, sprite1);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener->clone(), sprite2);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener->clone(), sprite3);
	return true;
}
//→                            //↑
//首先触摸屏幕的时候，获取的是openGL的坐标 ↓，首先得转换成 cocos2d 下的坐标 →
//然后获取触碰的Node的大小，再将坐标换算成Node坐标系里的坐标
//实现判断是否触碰在了Node上
//在Node对象中有几个函数可以做坐标转换。
//convertToNodeSpace方法可以把世界坐标转换到当前node的本地坐标系中
//convertToWorldSpace方法可以把基于当前node的本地坐标系下的坐标转换到世界坐标系中；
//convertTouchToNodeSpace这个函数可以把屏幕坐标系转换到GL坐标系，再转换到父节点的本地坐标下。
bool TowerPanelLayer::onTouchBegan(Touch *touch, Event *event)
{
	// 1 返回触摸事件当前作用的目标节点 
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	// 2 把touch对象中保存的屏幕坐标系转换到GL坐标系，再转换到目标节点的本地坐标下
	Point locationInNode = target->convertTouchToNodeSpace(touch);
	// 3 计算目标节点的矩形区域
	Size size = target->getContentSize();
	Rect rect = Rect(0, 0, size.width, size.height);
	// 4 通过containsPoint方法来判断触碰点在不在目标节点的矩形区域内，即是否被选中
	if (rect.containsPoint(locationInNode))
	{
		//碰到某选项时，重新设置其透明度，向玩家表明该项是被选中的
		target->setOpacity(180);
		return true;
	}
	return false;
}


void TowerPanelLayer::onTouchEnded(Touch* touch, Event* event)
{
	// 根据选择的目标确定炮塔的类型 
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	if (target == sprite1)
	{
		//设置选择的防御塔的类型
		chooseTowerType = ARROW_TOWER;
		//还原选中的防御塔的透明度
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

