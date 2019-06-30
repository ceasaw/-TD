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
		//ˢ�½�Ǯ
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
			// c_str()��������һ��ָ������C�ַ�����ָ��, �����뱾string����ͬ.  
			// ���һ������Ҫ��char*����������ʹ��c_str()����
			// ����c_str()���ص���һ����ʱָ�룬���ܶ�����в�����������ֱ��
			// string str = "1234";
			// char* a  = str.c_str();
			if (star > UserDefault::getInstance()->getIntegerForKey(instance->getCurrLevelFile().c_str()))
			{
				UserDefault::getInstance()->setIntegerForKey(instance->getCurrLevelFile().c_str(), star);
			}

			instance->clear();
			// Ӧ����ת���ɹ�����
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
	//��ӱ�����
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
	// 1 �Ѵ����Cocos2d����ϵ�ֱ�ת��ΪtiledMap������������ꡣ
	//   tiledMap�����(0, 0) ���������Ͻǣ������������(0 , 0)�����½ǡ�
	Point towerCoord = convertTotileCoord(position);
	Point matrixCoord = convertToMatrixCoord(position);
	// 2 �ֱ�����Ƭ��ȫ�ֱ�������±ꡣ����tiledMap��ÿһ����Ƭ��˵��������һ��ȫ�ֱ�ʶ����
	//   ��Ƭ��GID��Χ��������1��ʼ����Ƭ��ͼ��tile��������
	//   �õ�����Ƭ��GID�Ϳ��Ի�ȡ�ÿ���Ƭ��ȫֵ����
	int gid = bgLayer->getTileGIDAt(towerCoord);
	//����gid��Ӧ��Ƭ������
	auto tileTemp = map->getPropertiesForGID(gid).asValueMap();
	//MatrixIndex �Ǿ���Ĵ洢����������Ƭ�ı��
	int MatrixIndex = static_cast<int>(matrixCoord.y * MAP_WIDTH + matrixCoord.x);
	// ��ȡ��Ƭ��Ϣ������Ƭ��һ��canTouch���ԣ���ֵΪ1����Ҳ�����˴���Ƭ�Ͽ��Դ���������
	int TouchVaule = 0;
	if (tileTemp.empty())
	{
		TouchVaule = 0;
	}
	else
	{
		TouchVaule = tileTemp.at("canTouch").asInt(); //ת��������
	}
	//��������������ꡣ��ʵ����Ǹ��ѵ�ͼ����ת��Ϊ��Ļ����Ĺ��̣���Ҫע����ǣ�
	//��������ֵʱ����Ӧ�ü�ȥ֮ǰ���������ǲ��־��룬��������ȷ��׼ȷ��
	auto tileWidth = map->getContentSize().width / map->getMapSize().width;
	auto tileHeight = map->getContentSize().height / map->getMapSize().height;
	towerPos = Point((towerCoord.x * tileWidth) + tileWidth / 2 - offX, 
		map->getContentSize().height - (towerCoord.y * tileHeight) - tileHeight / 2);
	//�������ô��ǿյ�����������������������ô���ڸô���������ѡ�����
	//�����ڸô����һ����ʾ�����ͼƬ����ʱ���ͼƬ�ᱻ�Ƴ�
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
	//point.end()ָ�򲻴��ڵ�Ԫ�أ���point��Ԫ��Ϊ�յ�ʱ��
	//point.begin()== point.end()���ԣ���û�л�ȡ��������Ԫ�ص�ʱ��
	//Ҳ����
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

	// 1 �жϵ�ǰ��ʣ��ĵ��ˣ�����ӣ������Ƿ�Ϊ0��
	//   ���Ϊ0�����ʾ�ò�����ȫ��������ӵ��˳�����
	auto restEnemyNum = groupEnemy->getEnemyTotal();
	if (restEnemyNum <= 0) {
		groupEnemy->setIsFinishedAddGroup(true);
		return;
	}
	// 2 ��Ϊ�ú��������һ�����ˣ����˵�ʣ������Ҳ����٣�
	//   ���Դ˴���Ҫ��������ʣ����˵�����
	restEnemyNum--;
	groupEnemy->setEnemyTotal(restEnemyNum);
	// 3 ���ݵ�ǰ��������Ϣ���������Thief��Pirate��Bandit��
	//   �����Ӧ���˵�ͬʱ��Ҫ������������ֵ��
	Enemy* enemy = NULL;
	Enemy* boss = NULL;
	if (groupCounter == 4 && iscreate == false)
	{
		iscreate = true;
		auto size = Director::getInstance()->getWinSize();

		//��һ����ʾ��
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
		//�ṹ������
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
	// 4 �Ѵ����õĵ�����ӵ�������������ӵ������б��С�
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
			//�ɹ��������ټ��ٽ�Ǯ
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
		//�������ж��Ƿ���Ҫ��ײ���
		return;
	}
	// 1 �����ɾ���ӵ��͵��˵���ʱ����������
	//   �����ӵ��������Ҫ��ɾ��ʱ���ͻ�����ǲ��뵽��Щ������
	Vector<Enemy*> enemyNeedToDelete;
	Vector<Sprite*> bulletNeedToDelete;
	// 2 �����ӵ������������ӵ���ռ�ط�Χ���ӵ����ڵ�ԭ�ڵ��Ƿ������Ĳ�
	//�����ӵ���RectʱҪ����������ֵӳ�䵽�����㣨PlayerLayer���У�
	//��֤���������ͬһ�ӽڵ��ϡ�
	for (int i = 0; i < bulletVector.size(); i++)
	{
		auto  bullet = bulletVector.at(i);
		auto  bulletRect = Rect(bullet->getPositionX() + bullet->getParent()->getPositionX() - bullet->getContentSize().width / 2,
			bullet->getPositionY() + bullet->getParent()->getPositionY() - bullet->getContentSize().height / 2,
			bullet->getContentSize().width,
			bullet->getContentSize().height);
		// 3 ��������������������˵ķ�Χ��
		//   getBoundingBox�������ڻ�þ������ź���ת֮������д�С
		for (int j = 0; j < enemyVector.size(); j++)
		{
			if (bossVector.empty() == false)
			{
				boss = bossVector.at(0);
			}
			auto enemy = enemyVector.at(j);
			auto enemyRect = enemy->getBoundingBox();
			// 4 �ж��ӵ�������Ƿ��н���������ཻ����Ѹõ��˺��ӵ���ӵ���ɾ�����б��С�
			if (bulletRect.intersectsRect(enemyRect))
			{
				auto T = static_cast<TowerBase*>(bullet->getParent());
				auto demage = T->getLethality();
				//��ȡ��ǰѪ��
				auto currHp = enemy->getCurrHp();
				currHp-= demage;
				//��������Ѫ��
				enemy->setCurrHp(currHp);
				//��������Ѫ���İٷֱ�
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
				// 5 ֻҪ���е��ˣ�������ѭ������Ҳ��ζ��һ��bulletֻ�����һ�����ˡ�
				break;
			}
		}
		// 6 ���ٴ�ɾ���б��еĵ��ˣ����Ѹõ��˴�enemyVector���Ƴ����������enemyNeedToDelete��
		//   enemyExpload�������ڲ����˱�ըЧ�������ٵ���
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

			//�жϷ����Ĵ�������
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
	// 7 ��bulletVector���Ƴ���ɾ���б��е��ӵ��������������bulletNeedToDelete
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

	//// 1 ��ȡ��ǰ���εĵ�����Ϣ�����Ϊ�գ��򷵻ء�
	//auto groupEnemy = this->currentGroup();
	//if (groupEnemy == NULL) { return; }
	//// 2 ����ǰ���ĵ���ȫ������ӵ��˳�����
	////   ���Ҵ�ʱ�����е���������Ϊ0���������˳�����أ�ʱ�������һ���ĵ��ˡ�
	//if (groupEnemy->getIsFinishedAddGroup() == true 
	//	&& instance->enemyVector.size() == 0 
	//	&& groupCounter < instance->getGroupNum())
	//{
	//	groupEnemy = this->nextGroup();
	//}
	// 3 ��ӵ��ˡ�Ҳ����˵��ֻҪgroupEnemy��Ϊ�գ�
	//   ��ôÿdt��ͻ�ִ��һ��addEnemy��������ӵ���
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
				
				// Ӧ����ת��ʧ�ܽ���
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
	// ����������ͼƬ
	auto spritetool = Sprite::createWithSpriteFrameName("toolbg.png");
	spritetool->setAnchorPoint(Point(0.5f, 1));
	spritetool->setPosition(Point(size.width / 2, size.height));
	toolLayer->addChild(spritetool);
	// �����
	auto money = instance->getMoney();
	moneyLabel = Label::createWithBMFont("fonts/bitmapFontChinese.fnt", " ");
	moneyLabel->setPosition(Point(spritetool->getContentSize().width / 8, spritetool->getContentSize().height / 2));
	moneyLabel->setAnchorPoint(Point(0, 0.5f));
	auto moneyText = std::to_string(money);
	moneyLabel->setString(moneyText);
	spritetool->addChild(moneyLabel);
	// ���Ѫ����
	playHpBar = ProgressTimer::create(Sprite::createWithSpriteFrameName("playhp.png"));
	playHpBar->setType(ProgressTimer::Type::BAR);
	playHpBar->setMidpoint(Point(0, 0.4f));
	playHpBar->setBarChangeRate(Point(1, 0));
	playHpBar->setPercentage(playHpPercentage);
	playHpBar->setPosition(Point(spritetool->getContentSize().width / 5 * 4, spritetool->getContentSize().height / 2));
	spritetool->addChild(playHpBar);
	// ��ҵ÷ֱ��  
	auto star = Sprite::createWithSpriteFrameName("playstar.png");
	star->setPosition(Point(spritetool->getContentSize().width / 5 * 4, spritetool->getContentSize().height / 2));
	spritetool->addChild(star);
	// ��ǰ����
	int groupTotal = instance->getGroupNum();
	groupLabel = Label::createWithBMFont("fonts/bitmapFontChinese.fnt", " ");
	groupLabel->setPosition(Point(spritetool->getContentSize().width / 8 * 3, spritetool->getContentSize().height / 2));
	groupLabel->setAnchorPoint(Point(0.5f, 0.5f));
	auto groupInfoText = std::to_string(groupCounter + 1);
	groupLabel->setString(groupInfoText);
	spritetool->addChild(groupLabel);
	// �ܲ���
	groupTotalLabel = Label::createWithBMFont("fonts/bitmapFontChinese.fnt", " ");
	groupTotalLabel->setPosition(Point(spritetool->getContentSize().width / 2, spritetool->getContentSize().height / 2));
	groupTotalLabel->setAnchorPoint(Point(0.5f, 0.5f));
	auto groupTotalText = std::to_string(groupTotal);
	groupTotalLabel->setString(groupTotalText);
	spritetool->addChild(groupTotalLabel);
}
