#include "Enemy.h"

Enemy::Enemy()
	:pointCounter(0)
	, animationRight(NULL)
	, animationLeft(NULL)
	, pointsVector(NULL)
	, runSpeed(0)
	, maxHp(0)
	, currHp(0)
	, hpBar(NULL)
	, enemySuccessful(false)
	,hpPercentage(100.0)
{
}

//Enemy::Enemy(Enemy* & enemy)
//{
//	Point p = enemy->getPosition();
//	this->setPosition(p);
//}


bool Enemy::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	//addChild(sprite);
	return true;
}
                                                          //帧数			//延

//Animation * Enemy::createAnimation(std::string prefixName, int framesNum, float delay)
//{



	//for (int i = 1; i <= framesNum; i++)
	//   {
	//       char buffer[20] = { 0 };
	//       sprintf(buffer, "_%i.png",  i);
	//       std::string str =  prefixName + buffer;  //字符串连接如：enemyLeft1_1.png = enemyLeft1 + _1.png
	//       auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
	//       animFrames.pushBack(frame);
	//   }
//
Animation * Enemy::createAnimation(std::string prefixName, int framesNum, float delay)
{

	//创建精灵帧
	auto cashe = SpriteFrameCache::getInstance();    //缓存
													 //cashe->addSpriteFramesWithFile("Sprite.plist"); //默认加载
	Vector<SpriteFrame*> animFrames;
	for (int i = 1; i <= framesNum; i++)
	{
		auto frame = cashe->getSpriteFrameByName(prefixName + "_" + std::to_string(i) + ".png");
		animFrames.pushBack(frame);
	}
	//createWithSpriteFrames会返回Animation对象
	return Animation::createWithSpriteFrames(animFrames, delay);	// Vector<SpriteFrame*> animFrames;
}

void Enemy::changeDirection(float dt)
{
	//currPoint 是当前要到达的位置，所以永远在Monster的前面。
	auto curr = currPoint();
	if (curr->getPositionX() > this->getPosition().x)
	{                              //AnimationCache是动画缓存
		runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runright")));
	}
	else {
		runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runleft")));
	}
}

Node* Enemy::currPoint()
{
	//初始是0
	return this->pointsVector.at(pointCounter);
}

Node* Enemy::nextPoint()
{
	int maxCount = this->pointsVector.size();
	pointCounter++;
	//先判断是否有下个路径点，如果有就返回下个路径点
	if (pointCounter < maxCount) {
		auto node = this->pointsVector.at(pointCounter);
		return node;
	}
	else {
		//没有就将pointCounter返回原值，然后返回空
		//pointCounter = maxCount - 1;
		setEnemySuccessful(true);
	}
	return NULL;
}

void Enemy::runFllowPoint()
{
	//从当前的节点找到下一个节点
	auto point = currPoint();
	setPosition(point->getPosition());
	point = nextPoint();
	//指定的回调函数是：runFllowPoint本身。所以这个函数就会重复地调用自身，
	//	   不断地判断计算得到下一个路径点，让敌人MoveTo到那个路径点地位置
	if (point != NULL) {						//第一个参数是时间
		runAction(CCSequence::create(MoveTo::create(getRunSpeed(), point->getPosition())
			, CallFuncN::create(CC_CALLBACK_0(Enemy::runFllowPoint, this))
			, NULL));
	}
}

void Enemy::setPointsVector(Vector<Node*> points)
{
	this->pointsVector = points;
}

void Enemy::createAndSetHpBar()
{
	//将没有大小的enemy重新设置图画的实际大小
	sprite = Sprite::createWithSpriteFrameName("enemyRight2_1.png");
	Size size = sprite->getContentSize();
	this->setContentSize(size);
	// 1 添加血条背景图片。
	hpBgSprite = Sprite::createWithSpriteFrameName("hpBg1.png");
	//hpBgSprite->setPosition(Point(this->getContentSize().width / 2+30, this->getContentSize().height+60));
	hpBgSprite->setPosition(Point(this->getContentSize().width / 2 , this->getContentSize().height));
	
	// 2 添加血条进度条。
	//进度条ProgressTimer有两种类型：一种是环形，一种是条形（包括vertical 和 horizontal），
	//所以使用进度条时需要指明它是哪种类型。
	//setMidpoint方法设置进度条的起始点，（0，y）表示最左边，（1，y）表示最右边，（x，1）表示最上面，（x，0）表示最下面。
	//setBarChangeRate方法用来设置进度条变化方向的，如果不用变化的方向，则设置该方向为0，否则设置为1。
	//所以（1, 0）表示横方向，（0, 1）表示纵方向。
	//ProgressTimer有一个很最要的percentage属性。它代表了当前进度条的进度值。
	//如果要让一个进度条正常的显示出来，那么percentage的值必须大于0。
	//setPercentage方法能设置ProgressTimer的percentage值。
	hpBar = ProgressTimer::create(Sprite::createWithSpriteFrameName("hp1.png"));
	hpBar->setType(ProgressTimer::Type::BAR);
	hpBar->setMidpoint(Point(0, 0.5f));
	hpBar->setBarChangeRate(Point(1, 0));
	hpBar->setPercentage(hpPercentage);
	hpBar->setPosition(Point(hpBgSprite->getContentSize().width / 2, hpBgSprite->getContentSize().height / 3 * 2));
	hpBgSprite->addChild(hpBar);	
	this->addChild(hpBgSprite);
}

