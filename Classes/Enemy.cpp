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
                                                          //֡��			//��

//Animation * Enemy::createAnimation(std::string prefixName, int framesNum, float delay)
//{



	//for (int i = 1; i <= framesNum; i++)
	//   {
	//       char buffer[20] = { 0 };
	//       sprintf(buffer, "_%i.png",  i);
	//       std::string str =  prefixName + buffer;  //�ַ��������磺enemyLeft1_1.png = enemyLeft1 + _1.png
	//       auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
	//       animFrames.pushBack(frame);
	//   }
//
Animation * Enemy::createAnimation(std::string prefixName, int framesNum, float delay)
{

	//��������֡
	auto cashe = SpriteFrameCache::getInstance();    //����
													 //cashe->addSpriteFramesWithFile("Sprite.plist"); //Ĭ�ϼ���
	Vector<SpriteFrame*> animFrames;
	for (int i = 1; i <= framesNum; i++)
	{
		auto frame = cashe->getSpriteFrameByName(prefixName + "_" + std::to_string(i) + ".png");
		animFrames.pushBack(frame);
	}
	//createWithSpriteFrames�᷵��Animation����
	return Animation::createWithSpriteFrames(animFrames, delay);	// Vector<SpriteFrame*> animFrames;
}

void Enemy::changeDirection(float dt)
{
	//currPoint �ǵ�ǰҪ�����λ�ã�������Զ��Monster��ǰ�档
	auto curr = currPoint();
	if (curr->getPositionX() > this->getPosition().x)
	{                              //AnimationCache�Ƕ�������
		runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runright")));
	}
	else {
		runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runleft")));
	}
}

Node* Enemy::currPoint()
{
	//��ʼ��0
	return this->pointsVector.at(pointCounter);
}

Node* Enemy::nextPoint()
{
	int maxCount = this->pointsVector.size();
	pointCounter++;
	//���ж��Ƿ����¸�·���㣬����оͷ����¸�·����
	if (pointCounter < maxCount) {
		auto node = this->pointsVector.at(pointCounter);
		return node;
	}
	else {
		//û�оͽ�pointCounter����ԭֵ��Ȼ�󷵻ؿ�
		//pointCounter = maxCount - 1;
		setEnemySuccessful(true);
	}
	return NULL;
}

void Enemy::runFllowPoint()
{
	//�ӵ�ǰ�Ľڵ��ҵ���һ���ڵ�
	auto point = currPoint();
	setPosition(point->getPosition());
	point = nextPoint();
	//ָ���Ļص������ǣ�runFllowPoint����������������ͻ��ظ��ص�������
	//	   ���ϵ��жϼ���õ���һ��·���㣬�õ���MoveTo���Ǹ�·�����λ��
	if (point != NULL) {						//��һ��������ʱ��
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
	//��û�д�С��enemy��������ͼ����ʵ�ʴ�С
	sprite = Sprite::createWithSpriteFrameName("enemyRight2_1.png");
	Size size = sprite->getContentSize();
	this->setContentSize(size);
	// 1 ���Ѫ������ͼƬ��
	hpBgSprite = Sprite::createWithSpriteFrameName("hpBg1.png");
	//hpBgSprite->setPosition(Point(this->getContentSize().width / 2+30, this->getContentSize().height+60));
	hpBgSprite->setPosition(Point(this->getContentSize().width / 2 , this->getContentSize().height));
	
	// 2 ���Ѫ����������
	//������ProgressTimer���������ͣ�һ���ǻ��Σ�һ�������Σ�����vertical �� horizontal����
	//����ʹ�ý�����ʱ��Ҫָ�������������͡�
	//setMidpoint�������ý���������ʼ�㣬��0��y����ʾ����ߣ���1��y����ʾ���ұߣ���x��1����ʾ�����棬��x��0����ʾ�����档
	//setBarChangeRate�����������ý������仯����ģ�������ñ仯�ķ��������ø÷���Ϊ0����������Ϊ1��
	//���ԣ�1, 0����ʾ�᷽�򣬣�0, 1����ʾ�ݷ���
	//ProgressTimer��һ������Ҫ��percentage���ԡ��������˵�ǰ�������Ľ���ֵ��
	//���Ҫ��һ����������������ʾ��������ôpercentage��ֵ�������0��
	//setPercentage����������ProgressTimer��percentageֵ��
	hpBar = ProgressTimer::create(Sprite::createWithSpriteFrameName("hp1.png"));
	hpBar->setType(ProgressTimer::Type::BAR);
	hpBar->setMidpoint(Point(0, 0.5f));
	hpBar->setBarChangeRate(Point(1, 0));
	hpBar->setPercentage(hpPercentage);
	hpBar->setPosition(Point(hpBgSprite->getContentSize().width / 2, hpBgSprite->getContentSize().height / 3 * 2));
	hpBgSprite->addChild(hpBar);	
	this->addChild(hpBgSprite);
}

