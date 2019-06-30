#pragma once
#ifndef _PLAY_LAYER_H_
#define _PLAY_LAYER_H_

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocostudio::timeline;
using namespace cocos2d::ui;

class TowerPanelLayer;
class GroupEnemy;
class TowerBase;
class Enemy;

class PlayLayer : public Layer
{
public:
	PlayLayer();
	~PlayLayer();

	virtual bool init() override;
	static Scene* createScene();
	CREATE_FUNC(PlayLayer);

private:
	void update(float dt) override; //�ڴ˴��ж����Ƿ�ɹ�

	bool iscreate;
	int groupCounter;//���˲���
	float offX;
	TowerPanelLayer* chooseTowerpanle;
	SpriteBatchNode *spriteSheet;
	TMXTiledMap* map;
	TMXLayer* bgLayer;
	TMXObjectGroup* objects;
	Vector<Node*> pointsVector; //enemy�ƶ�·���ĵ㼯

	void addTowerChoosePanle(Point point);
	Point towerPos;
	Point convertTotileCoord(Point position);  // �ѱ������꣨OpenGL���꣩ת��Ϊ��ͼ����
	Point convertToMatrixCoord(Point position);// �ѱ������꣨OpenGL���꣩ת��Ϊ��������
	void checkAndAddTowerPanle(Point position);
	void initPointsVector(float offX);
	void addEnemy();
	void addTower();
	//void ChioceLayerTouch(Ref* pSender, Widget::TouchEventType type);
	bool onTouchBegan(Touch * touch, Event * event);
	void CollisionDetection(); //��ײ��⣬��ͬ�ڵ�����ת��
	TowerBase **towerMatrix;

	GroupEnemy* currentGroup();
	GroupEnemy* groupEnemy;
	GroupEnemy* nextGroup();
	void logic(float dt);

	Enemy* boss;

	Label* moneyLabel;
	Label* startLabel;
	Label* groupLabel;
	Label* groupTotalLabel;
	Label* bossComingLabel;
	Layer* toolLayer;
	bool isSuccessful;
	void enemyIntoHouse();
	void InitToolLayer();

	CC_SYNTHESIZE(float, playHpPercentage, PlayHpPercentage);
	CC_SYNTHESIZE_READONLY(ProgressTimer*, playHpBar, PlayHpBar);

};





#endif // !_PLAY_LAYER_H_
