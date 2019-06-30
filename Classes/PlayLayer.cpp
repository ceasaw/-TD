#define MAP_WIDTH 16
#define MAP_HEIGHT 9

#include "PlayLayer.h"
#include "Enemy.h"
#include "Thief.h"
#include "Priate.h"
#include "Bandit.h"
#include "ArrowTower.h"
#include "AttackTower.h"
#include "MultiDirTower.h"
#include "GameManager.h"
#include "TowerPanelLayer.h"
#include "GroupEnemy.h"
#include "LoadLevelInfo.h"
#include "SimpleAudioEngine.h"
#include "CrazyDragon.h"

using namespace CocosDenshion;

PlayLayer::PlayLayer()
	:spriteSheet(NULL)      , map(NULL)
	, objects(NULL)         , pointsVector(NULL)
	, towerMatrix(NULL)     , offX(0.0)
	, groupCounter(0)       , chooseTowerpanle(NULL)
	, groupEnemy(NULL)      , isSuccessful(false)
	,moneyLabel(NULL)       ,groupLabel(NULL)
	,groupTotalLabel(NULL)	,toolLayer(NULL)
	,playHpPercentage(100)  ,playHpBar(NULL)
	,boss(NULL)             , iscreate(false)
{
}

PlayLayer::~PlayLayer()
{
	if (towerMatrix) {
		free(towerMatrix);
	}
	pointsVector.clear();
}

Scene *PlayLayer::createScene()
{
	
    auto scene = Scene::create();
    auto layer = PlayLayer::create();
    scene->addChild(layer);
    return scene;
}

void PlayLayer::update(float dt)
{
	GameManager* instance = GameManager::getInstance();
	if(this!= NULL)
	{
		addTower();
		CollisionDetection();
		enemyIntoHouse();
		//刷新金钱
		if (moneyLabel != NULL)
		{
			auto money = instance->getMoney();
			auto moneyText = std::to_string(money);
			moneyLabel->setString(moneyText);
		}


		if (isSuccessful == true)
		{
			isSuccessful = false;
			auto star = 0;
			auto playHp = this->getPlayHpPercentage();

			if (playHp > 0 && playHp <= 30) { star = 1; }
			else if (playHp > 30 && playHp <= 60) { star = 2; }
			else if (playHp > 60 && playHp <= 100) { star = 3; }
			// c_str()函数返回一个指向正规C字符串的指针, 内容与本string串相同.  
			// 如果一个函数要求char*参数，可以使用c_str()方法
			// 由于c_str()返回的是一个临时指针，不能对其进行操作，不可以直接
			// string str = "1234";
			// char* a  = str.c_str();
			if (star > UserDefault::getInstance()->getIntegerForKey(instance->getCurrLevelFile().c_str()))
			{
				UserDefault::getInstance()->setIntegerForKey(instance->getCurrLevelFile().c_str(), star);
			}

			instance->clear();
			// 应该跳转到成功界面
			Size winSize = Director::getInstance()->getWinSize();
			auto putOutLabel = Label::createWithBMFont("fonts/boundsTestFont.fnt", "Congratulations!");
			putOutLabel->setPosition(Point(winSize.width / 2, winSize.height / 2));
			auto seq = Sequence::create(ScaleTo::create(4, 2),
				ScaleTo::create(1, 2),
				FadeOut::create(3),
				CallFuncN::create([=](Ref* pSender)
			{
				PostQuitMessage(0);
			}),NULL);

			//putOutLabel->setScale(4);
			this->addChild(putOutLabel, 100);
			putOutLabel->runAction(seq);
		}
	}
	
}

bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    Size winSize = Director::getInstance()->getWinSize();
	startLabel = Label::createWithBMFont("fonts/boundsTestFont.fnt", "GameStart!");
	do 
	{
		startLabel->setScale(5);
		startLabel->setPosition(Point(winSize.width / 2, winSize.height / 2));
		startLabel->runAction(Sequence::create(
			FadeOut::create(5),
			CallFuncN::create([=](Node* pSender)
		{
			startLabel->removeFromParentAndCleanup(true);
			startLabel = NULL;
		}), NULL));
		this->addChild(startLabel, 50);
	} while (0);

	auto info = LoadLevelinfo::createLoadLevelinfo("levelInfo_1_0.plist");
	info->readLevelInfo();

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Play.plist");
	//添加背景层
	auto gameBg = Sprite::create("playbg.png");
	gameBg->setPosition(Point(winSize.width / 2, winSize.height / 2));
	addChild(gameBg, -20);

    spriteSheet = SpriteBatchNode::create("Play.png");
    addChild(spriteSheet);
    
    map = TMXTiledMap::create("map.tmx");
    bgLayer = map->getLayer("bg");

    bgLayer->setAnchorPoint(Point(0.5f, 0.5f));
    bgLayer->setPosition(Point(winSize.width / 2 ,winSize.height / 2));
	objects = map->getObjectGroup("obj");
    this->addChild(map, -1);

	InitToolLayer();

    offX = ( map->getContentSize().width - winSize.width )/ 2;
    initPointsVector(offX);

	scheduleUpdate();
	//this->schedule(schedule_selector(PlayLayer::addTower),0.5f);
	schedule(schedule_selector(PlayLayer::logic), 2.0f);
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	
	int arraySize = sizeof(TowerBase *) * MAP_WIDTH * MAP_HEIGHT;
	towerMatrix = (TowerBase **)malloc(arraySize);
	memset((void*)towerMatrix, 0, arraySize);

	for (int row = 0; row < MAP_HEIGHT; row++) {
		for (int col = 0; col < MAP_WIDTH; col++) {
			towerMatrix[row * MAP_WIDTH + col] = NULL;
		}
	}
    return true;
}

void PlayLayer::addTowerChoosePanle(Point point)
{

	chooseTowerpanle = TowerPanelLayer::create();
	chooseTowerpanle->setPosition(point);
	this->addChild(chooseTowerpanle,100);
}

Point PlayLayer::convertTotileCoord(Point position)
{
	int x = (position.x + offX) / map->getContentSize().width * map->getMapSize().width;
	int y = map->getMapSize().height - position.y / map->getContentSize().height * map->getMapSize().height;
	return Point(x, y);
}

Point PlayLayer::convertToMatrixCoord(Point position)
{
	int x = (position.x + offX) / map->getContentSize().width * map->getMapSize().width;
	int y = position.y / map->getContentSize().height * map->getMapSize().height;
	return Point(x, y);
}

void PlayLayer::checkAndAddTowerPanle(Point position)
{
	// 1 把传入的Cocos2d坐标系分别转换为tiledMap坐标和数组坐标。
	//   tiledMap坐标的(0, 0) 坐标在左上角，而数组坐标的(0 , 0)在左下角。
	Point towerCoord = convertTotileCoord(position);
	Point matrixCoord = convertToMatrixCoord(position);
	// 2 分别求瓦片的全局标和数组下标。对于tiledMap的每一个瓦片来说，它都有一个全局标识量，
	//   瓦片的GID范围从正整数1开始到瓦片地图中tile的总量。
	//   得到了瓦片的GID就可以获取该块瓦片的全值在左。
	int gid = bgLayer->getTileGIDAt(towerCoord);
	//返回gid对应瓦片的属性
	auto tileTemp = map->getPropertiesForGID(gid).asValueMap();
	//MatrixIndex 是具体的存储防御塔的瓦片的标号
	int MatrixIndex = static_cast<int>(matrixCoord.y * MAP_WIDTH + matrixCoord.x);
	// 获取瓦片信息。该瓦片有一个canTouch属性，其值为1，这也表明了此瓦片上可以创建炮塔。
	int TouchVaule = 0;
	if (tileTemp.empty())
	{
		TouchVaule = 0;
	}
	else
	{
		TouchVaule = tileTemp.at("canTouch").asInt(); //转换成整形
	}
	//修正炮塔面板坐标。其实这就是个把地图坐标转换为屏幕坐标的过程，需要注意的是，
	//计算坐标值时我们应该减去之前修正误差的那部分距离，这样才能确保准确。
	auto tileWidth = map->getContentSize().width / map->getMapSize().width;
	auto tileHeight = map->getContentSize().height / map->getMapSize().height;
	towerPos = Point((towerCoord.x * tileWidth) + tileWidth / 2 - offX, 
		map->getContentSize().height - (towerCoord.y * tileHeight) - tileHeight / 2);
	//如果满足该处是空地且无其他炮塔的条件，那么则在该处创建炮塔选择界面
	//否则在该处添加一个提示错误的图片，不时这个图片会被移除
	if (TouchVaule == 1 && towerMatrix[MatrixIndex] == NULL)
	{
		addTowerChoosePanle(towerPos);
	}
	else if(TouchVaule != 1)
	{
		auto tips = Sprite::createWithSpriteFrameName("no.png");
		tips->setPosition(towerPos);
		this->addChild(tips);
		tips->runAction(Sequence::create(DelayTime::create(0.8f),
			FadeOut::create(0.5f),
			CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, tips)),
			NULL));
	}
}

void PlayLayer::initPointsVector(float offX)
{
    Node *runOfPoint = NULL;
	int count = 0;
	ValueMap point;
	point = objects->getObject(std::to_string(count));
	//point.end()指向不存在的元素，当point中元素为空的时候，
	//point.begin()== point.end()所以，当没有获取到对象层的元素的时候，
	//也就是
	while (point.begin()!= point.end())
	{
		float x = point.at("x").asFloat();
		float y = point.at("y").asFloat();
		runOfPoint = Node::create();
		runOfPoint->setPosition(Point(x - offX , y  ));
		this->pointsVector.pushBack(runOfPoint);
		count++;
		point = objects->getObject( std::to_string(count));
	}
	runOfPoint = NULL;
}

void PlayLayer::addEnemy()
{
	GameManager *instance = GameManager::getInstance();
	GroupEnemy* groupEnemy = this->currentGroup();
	if (groupEnemy == NULL)
	{
		return;
	}

	// 1 判断当前波剩余的敌人（待添加）总数是否为0，
	//   如果为0，则表示该波敌人全部都被添加到了场景。
	auto restEnemyNum = groupEnemy->getEnemyTotal();
	if (restEnemyNum <= 0) {
		groupEnemy->setIsFinishedAddGroup(true);
		return;
	}
	// 2 因为该函数会添加一个敌人，敌人的剩余总数也会减少，
	//   所以此处需要重新设置剩余敌人的总数
	restEnemyNum--;
	groupEnemy->setEnemyTotal(restEnemyNum);
	// 3 根据当前波敌人信息，依次添加Thief，Pirate和Bandit。
	//   添加相应敌人的同时需要减少它的总数值。
	Enemy* enemy = NULL;
	Enemy* boss = NULL;
	if (groupCounter == 4 && iscreate == false)
	{
		iscreate = true;
		auto size = Director::getInstance()->getWinSize();

		//加一个提示层
		bossComingLabel = Label::createWithBMFont("fonts/boundsTestFont.fnt", "BOSS IS COMING!!!");
		bossComingLabel->setScale(0);
		bossComingLabel->setPosition(Point(size.width / 2, size.height / 2));
		bossComingLabel->runAction(Sequence::create(ScaleTo::create(3, 4),
			ScaleTo::create(1, 1.5),
			FadeOut::create(2),
			CallFuncN::create([=] (Node* pSender)
		{
			bossComingLabel->removeFromParentAndCleanup(true);
			bossComingLabel = NULL;
		}),NULL));
		this->addChild(bossComingLabel, 100);
		boss = CrazyDragon::createCrazyDragon(pointsVector, 1000);

		this->addChild(boss,11);
		//结构有问题
		instance->enemyVector.pushBack(boss);
		instance->bossVector.pushBack(boss);
	}
	if (groupEnemy->getType1Total() > 0) {
		enemy = Thief::createThief(pointsVector, groupEnemy->getType1Hp());
		groupEnemy->setType1Total(groupEnemy->getType1Total() - 1);
	}
	else if (groupEnemy->getType2Total() > 0) {
		enemy = Pirate::createPirate(pointsVector, groupEnemy->getType2Hp());
		groupEnemy->setType2Total(groupEnemy->getType2Total() - 1);
	}
	else if (groupEnemy->getType3Total() > 0) {
		enemy = Bandit::createBandit(pointsVector, groupEnemy->getType3Hp());
		groupEnemy->setType3Total(groupEnemy->getType3Total() - 1);
	}
	// 4 把创建好的敌人添加到场景，并且添加到敌人列表中。
	this->addChild(enemy, 10);
	instance->enemyVector.pushBack(enemy);
	 
}

void PlayLayer::addTower()
{
	GameManager* instance = GameManager::getInstance();
	auto money = instance->getMoney();
	if (chooseTowerpanle != NULL)
	{
		auto type = chooseTowerpanle->getChooseTowerType();
		if (type == TowerType::ANOTHER)
		{
			return;
		}
		Point matrixCoord = convertToMatrixCoord(towerPos);
		int MatrixIndex = static_cast<int>(matrixCoord.y * MAP_WIDTH + matrixCoord.x);
		bool noMoneyTips = false;
		TowerBase* tower = NULL;
		if (type == TowerType::ARROW_TOWER)
		{
			auto arrow = ArrowTower::create();
			auto value = arrow->getTowerValue();
			if (money >= value)
			{
				tower = ArrowTower::create();
				money -= value;
			}
			else
				noMoneyTips = true;
		}
		else if (type == TowerType::ATTACK_TOWER)
		{
			auto arrow = AttackTower::create();
			auto value = arrow->getTowerValue();
			if (money >= value)
			{
				tower = AttackTower::create();
				money -= value;
			}
			else
				noMoneyTips = true;
		}
		else if (type == TowerType::MULTIDIR_TOWER)
		{
			auto arrow = MultiDirTower::create();
			auto value = arrow->getTowerValue();
			if (money >= value)
			{
				tower = MultiDirTower::create();
				money -= value;
			}
			else 
			{
				noMoneyTips = true;
			}
				
		}
		
		if (tower != NULL)
		{
			tower->setPosition(towerPos);
			//成功创建后再减少金钱
			instance->setMoney(money);
			tower->runAction(Sequence::create(FadeIn::create(1.0f), NULL));
			this->addChild(tower,1);
			towerMatrix[MatrixIndex] = tower;
		}
		type = TowerType::ANOTHER;
		chooseTowerpanle->setChooseTowerType(type);
		chooseTowerpanle->removeFromParentAndCleanup(true);
		chooseTowerpanle = NULL;

		if (noMoneyTips == true)
		{
			auto tips = Sprite::createWithSpriteFrameName("nomoney_mark.png");
			tips->setPosition(towerPos);
			this->addChild(tips);
			tips->runAction(Sequence::create(DelayTime::create(0.5f),
				CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, tips)),
				NULL));
		}
	}
}

//void PlayLayer::ChioceLayerTouch(Ref * pSender, Widget::TouchEventType type)
//{
//	switch (type)
//	{
//	case Widget::TouchEventType::BEGAN:
//		auto director = Director::getInstance();
//		auto layer = chooseTowerpanle->create();
//		this->addChild(layer, 10);
//		Size size = Director::getInstance()->getWinSize();
//		layer->setPosition(size.width / 2, size.height / 2);
//		CCLOG("Began");
//		break;
//	}
//}

bool PlayLayer::onTouchBegan(Touch * touch, Event * event)
{
	if (chooseTowerpanle != NULL)
	{
		this->removeChild(chooseTowerpanle);
		chooseTowerpanle = NULL;
	}
	auto location = touch->getLocation();
	checkAndAddTowerPanle(location);

	return true;
}

void PlayLayer::CollisionDetection()
{
	GameManager *instance = GameManager::getInstance();
	auto bulletVector = instance->bulletVector;
	auto enemyVector = instance->enemyVector;
	auto bossVector = instance->bossVector;
	if ( bulletVector.empty() || enemyVector.empty())
	{
		//首先是判断是否需要碰撞检测
		return;
	}
	// 1 定义待删除子弹和敌人的临时向量变量，
	//   当有子弹或敌人需要被删除时，就会把它们插入到这些向量中
	Vector<Enemy*> enemyNeedToDelete;
	Vector<Sprite*> bulletNeedToDelete;
	// 2 遍历子弹向量，计算子弹的占地范围，子弹所在的原节点是防御塔的层
	//其中子弹的Rect时要把它的坐标值映射到场景层（PlayerLayer）中，
	//保证能与敌人在同一子节点上。
	for (int i = 0; i < bulletVector.size(); i++)
	{
		auto  bullet = bulletVector.at(i);
		auto  bulletRect = Rect(bullet->getPositionX() + bullet->getParent()->getPositionX() - bullet->getContentSize().width / 2,
			bullet->getPositionY() + bullet->getParent()->getPositionY() - bullet->getContentSize().height / 2,
			bullet->getContentSize().width,
			bullet->getContentSize().height);
		// 3 遍历敌人向量，计算敌人的范围。
		//   getBoundingBox函数用于获得经过缩放和旋转之后的外框盒大小
		for (int j = 0; j < enemyVector.size(); j++)
		{
			if (bossVector.empty() == false)
			{
				boss = bossVector.at(0);
			}
			auto enemy = enemyVector.at(j);
			auto enemyRect = enemy->getBoundingBox();
			// 4 判断子弹与敌人是否有交集。如果相交，则把该敌人和子弹添加到待删除的列表中。
			if (bulletRect.intersectsRect(enemyRect))
			{
				auto T = static_cast<TowerBase*>(bullet->getParent());
				auto demage = T->getLethality();
				//获取当前血条
				auto currHp = enemy->getCurrHp();
				currHp-= demage;
				//重新设置血条
				enemy->setCurrHp(currHp);
				//重新设置血条的百分比
				auto currHpPercentage = enemy->getHpPercentage();
				auto MaxHp = enemy->getMaxHp();
				auto offHp =(demage)/ static_cast<float>(MaxHp);
				//auto offHp = 100 / enemy->getMaxHp();
				currHpPercentage -= offHp*100;
				if (currHpPercentage < 0 ) 
				{
					currHpPercentage = 0;	
				}
				enemy->setHpPercentage(currHpPercentage);
				enemy->getHpBar()->setPercentage(currHpPercentage);
				if (currHp <= 0)
				{
					auto isdead = enemy->getIsDead();
					enemy->setIsDead(++isdead);
					enemyNeedToDelete.pushBack(enemy);
				}
				//if (boss != NULL)
				//{
				//	auto BosscurHp = boss->getCurrHp();
				//	BosscurHp -= demage;
				//	boss->setCurrHp(BosscurHp);
				//	auto BosscurrHpPercentage = boss->getHpPercentage();
				//	auto BossMaxHp = boss->getMaxHp();
				//	auto BossoffHp = (demage) / static_cast<float>(BossMaxHp);
				//	BosscurrHpPercentage -= BossoffHp * 100;
				//	if (BosscurrHpPercentage < 0)
				//	{
				//		BosscurrHpPercentage = 0;
				//	}
				//	boss->setHpPercentage(BosscurrHpPercentage);
				//	boss->getHpBar()->setPercentage(BosscurrHpPercentage);
				//	if (BosscurHp <= 0)
				//	{
				//		auto isdead = boss->getIsDead();
				//		boss->setIsDead(++isdead);
				//		enemyNeedToDelete.pushBack(boss);
				//	}
				//}
				bulletNeedToDelete.pushBack(bullet);
				// 5 只要击中敌人，就跳出循环，这也意味着一个bullet只能射击一个敌人。
				break;
			}
		}
		// 6 销毁待删除列表中的敌人，并把该敌人从enemyVector中移除，最后清理enemyNeedToDelete。
		//   enemyExpload方法将在播放了爆炸效果后销毁敌人
		for (Enemy* enemyTemp : enemyNeedToDelete)
		{
			auto type = enemyTemp->getEnemyType();
			auto isdead = enemyTemp->getIsDead();
			if (type == BANDIT && isdead == 1)
			{
				auto money = instance->getMoney();
				money += 15;
				instance->setMoney(money);
			}
			else if (type == PRIATE && isdead == 1)
			{
				auto money = instance->getMoney();
				money += 20;
				instance->setMoney(money);
			}

			else if (type == THIEF && isdead == 1)
			{
				auto money = instance->getMoney();
				money += 30;
				instance->setMoney(money);
			}

			enemyTemp->enemyExpload();
			SimpleAudioEngine::getInstance()->playEffect("Sound/dead.mp3",false);
			instance->enemyVector.eraseObject(enemyTemp);

			//判断疯龙的触发机制
			if (enemyTemp->getEnemyType()!= CRAZYDRAGON && boss != NULL)
			{
				Point bossPosition = boss->getPosition();
				Point enemyPosition = enemyTemp->getPosition();
				auto distance = bossPosition.getDistance(enemyPosition);
				if (distance <= 500)
				{
					static_cast<CrazyDragon*>(boss)->setIsSpeedUp(true);
				}
			}
		}
		enemyNeedToDelete.clear();
	}
	// 7 从bulletVector中移除待删除列表中的子弹并销毁最后清理bulletNeedToDelete
	for (const auto& bulletTemp : bulletNeedToDelete)
	{
		instance->bulletVector.eraseObject(bulletTemp);
		bulletTemp->removeFromParent();
	}
	bulletNeedToDelete.clear();
}

GroupEnemy * PlayLayer::currentGroup()
{
	GameManager *instance = GameManager::getInstance();
	GroupEnemy* groupEnemy;
	if (!instance->groupVector.empty())
	{
		groupEnemy = (GroupEnemy*)instance->groupVector.at(groupCounter);
	}
	else
		groupEnemy = NULL;
	return groupEnemy;
}

GroupEnemy * PlayLayer::nextGroup()
{
	GameManager *instance = GameManager::getInstance();
	if (instance->groupVector.empty())
	{
		return NULL;
	}

	if (groupCounter < instance->getGroupNum() - 1)
	{
		groupCounter++;
		//SimpleAudioEngine::getInstance()->playEffect("Sound_WaveIncoming.wav",false);
	}
	else {
		isSuccessful = true;
	}
	GroupEnemy* groupEnemy = (GroupEnemy*)instance->groupVector.at(groupCounter);
	return groupEnemy;
}

void PlayLayer::logic(float dt)
{
	GameManager *instance = GameManager::getInstance();
	GroupEnemy* groupEnemy = this->currentGroup();

	if (groupEnemy == NULL)
	{
		return;
	}
	if (groupEnemy->getIsFinishedAddGroup() == true && instance->enemyVector.size() == 0 && groupCounter < instance->getGroupNum())
	{
		groupEnemy = this->nextGroup();
		int groupTotal = instance->getGroupNum();
		auto groupInfoText = std::to_string(groupCounter + 1);
		groupLabel->setString(groupInfoText);
		auto groupTotalText = std::to_string(groupTotal);
		groupTotalLabel->setString(groupTotalText);
	}
	this->addEnemy();

	//// 1 获取当前波次的敌人信息，如果为空，则返回。
	//auto groupEnemy = this->currentGroup();
	//if (groupEnemy == NULL) { return; }
	//// 2 当当前波的敌人全部都添加到了场景，
	////   并且此时场景中敌人数量变为0（被消灭或顺利过关）时，添加下一波的敌人。
	//if (groupEnemy->getIsFinishedAddGroup() == true 
	//	&& instance->enemyVector.size() == 0 
	//	&& groupCounter < instance->getGroupNum())
	//{
	//	groupEnemy = this->nextGroup();
	//}
	// 3 添加敌人。也就是说，只要groupEnemy不为空，
	//   那么每dt秒就会执行一次addEnemy方法来添加敌人
	/*this->addEnemy();*/
}

void PlayLayer::enemyIntoHouse()
{
	GameManager* instance = GameManager::getInstance();
	auto enemyVector = instance->enemyVector;
	for (int i = 0; i < enemyVector.size(); i++)
	{
		auto enemy = enemyVector.at(i);
		if (enemy->getEnemySuccessful())
		{
			instance->enemyVector.eraseObject(enemy);
			enemy->removeFromParent();
			auto playHp = getPlayHpPercentage() - 10;
			if (playHp > 0) {
				setPlayHpPercentage(playHp);
				playHpBar->setPercentage(playHp);
			}
			else 
			{
				
				// 应该跳转到失败界面
				this->unscheduleAllSelectors();
				delete moneyLabel;
				moneyLabel = NULL;

				for (Enemy* enemyTemp : enemyVector)
				{
					enemyTemp->stopAllActions();
				}
				auto TowerVector = instance->towerVector;

				for (TowerBase* towertemp : TowerVector)
				{
					towertemp->stopAllActions();
				}
				instance->clear();
				Size winSize = Director::getInstance()->getWinSize();
				auto putOutLabel = Label::createWithBMFont("fonts/boundsTestFont.fnt", "Game Over");
				putOutLabel->setPosition(Point(winSize.width / 2, winSize.height / 2));
				putOutLabel->setScale(4);
				this->addChild(putOutLabel,100);	
				this->runAction(Sequence::create(DelayTime::create(5.0f), 
					CallFuncN::create([=](Ref* pSender) 
				{ PostQuitMessage(0);
				}),NULL));
			}
		}
	}
}

void PlayLayer::InitToolLayer()
{
	GameManager* instance = GameManager::getInstance();
	auto size = Director::getInstance()->getWinSize();
	toolLayer = Layer::create();
	addChild(toolLayer);
	// 工具栏背景图片
	auto spritetool = Sprite::createWithSpriteFrameName("toolbg.png");
	spritetool->setAnchorPoint(Point(0.5f, 1));
	spritetool->setPosition(Point(size.width / 2, size.height));
	toolLayer->addChild(spritetool);
	// 金币数
	auto money = instance->getMoney();
	moneyLabel = Label::createWithBMFont("fonts/bitmapFontChinese.fnt", " ");
	moneyLabel->setPosition(Point(spritetool->getContentSize().width / 8, spritetool->getContentSize().height / 2));
	moneyLabel->setAnchorPoint(Point(0, 0.5f));
	auto moneyText = std::to_string(money);
	moneyLabel->setString(moneyText);
	spritetool->addChild(moneyLabel);
	// 玩家血量条
	playHpBar = ProgressTimer::create(Sprite::createWithSpriteFrameName("playhp.png"));
	playHpBar->setType(ProgressTimer::Type::BAR);
	playHpBar->setMidpoint(Point(0, 0.4f));
	playHpBar->setBarChangeRate(Point(1, 0));
	playHpBar->setPercentage(playHpPercentage);
	playHpBar->setPosition(Point(spritetool->getContentSize().width / 5 * 4, spritetool->getContentSize().height / 2));
	spritetool->addChild(playHpBar);
	// 玩家得分标尺  
	auto star = Sprite::createWithSpriteFrameName("playstar.png");
	star->setPosition(Point(spritetool->getContentSize().width / 5 * 4, spritetool->getContentSize().height / 2));
	spritetool->addChild(star);
	// 当前波数
	int groupTotal = instance->getGroupNum();
	groupLabel = Label::createWithBMFont("fonts/bitmapFontChinese.fnt", " ");
	groupLabel->setPosition(Point(spritetool->getContentSize().width / 8 * 3, spritetool->getContentSize().height / 2));
	groupLabel->setAnchorPoint(Point(0.5f, 0.5f));
	auto groupInfoText = std::to_string(groupCounter + 1);
	groupLabel->setString(groupInfoText);
	spritetool->addChild(groupLabel);
	// 总波数
	groupTotalLabel = Label::createWithBMFont("fonts/bitmapFontChinese.fnt", " ");
	groupTotalLabel->setPosition(Point(spritetool->getContentSize().width / 2, spritetool->getContentSize().height / 2));
	groupTotalLabel->setAnchorPoint(Point(0.5f, 0.5f));
	auto groupTotalText = std::to_string(groupTotal);
	groupTotalLabel->setString(groupTotalText);
	spritetool->addChild(groupTotalLabel);
}
