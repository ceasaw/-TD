#ifndef __THESTARTSCENE_H__
#define __THESTARTSCENE_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "PlayLayer.h"

USING_NS_CC;
using namespace cocostudio::timeline;
using namespace cocos2d::ui;

class StartScene : public cocos2d::Layer
{
public:

	static cocos2d::Scene* createScene();


	virtual bool init();
private:
	void StartGameTouch(Ref* pSender, Widget::TouchEventType type);  //�Զ����¼��ĺ�������
	Layout* root;  //��private����ó�����Ԫ��
	ImageView* Logo;
	ImageView* Shen;
	ImageView* Meng;

	Button* Start;
	Button* Save;
	Button* Exit;
	CREATE_FUNC(StartScene);
};

#endif // __THESTARTSCENE_H__
