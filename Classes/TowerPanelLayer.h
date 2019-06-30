#pragma once

#ifndef _TOWER_PANEL_LAYER_H_
#define _TOWER_PANEL_LAYER_H_

#include "cocos2d.h"

USING_NS_CC;

typedef enum TowerType
{
	ARROW_TOWER = 0,
	ATTACK_TOWER,
	MULTIDIR_TOWER,
	ANOTHER
};

class TowerPanelLayer : public Layer
{
public:
	virtual bool init() override;
	CREATE_FUNC(TowerPanelLayer);
	// ���ش����ص�����
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch* touch, Event* event);
	CC_SYNTHESIZE(TowerType, chooseTowerType, ChooseTowerType);//ѡ�����������

private:
	// �ֱ��ʾ���������������෽�򹥻���
	Sprite* sprite1;
	Sprite* sprite2;
	Sprite* sprite3;
};
#endif // !_TOWER_PANEL_LAYER_H_
