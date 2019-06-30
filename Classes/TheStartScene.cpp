#include "TheStartScene.h"


Scene* StartScene::createScene()
{
	auto scene = Scene::create();

	auto layer = StartScene::create();

	scene->addChild(layer);

	return scene;
}

bool StartScene::init()
{

	if (!Layer::init())
	{
		return false;
	}

	auto rootNode = CSLoader::createNode("MainScene/MainScene.csb");
	//��ȡ����Ԫ��
	Layout* root = (Layout*)rootNode->getChildByName("root"); //���ڵ㣬��������

	Start = (Button*)Helper::seekWidgetByName(root, "Start");
	Save = (Button*)Helper::seekWidgetByName(root, "Save");
	Exit = (Button*)Helper::seekWidgetByName(root, "Exit");
	Start->addTouchEventListener(CC_CALLBACK_2(StartScene::StartGameTouch, this)); //����Զ����¼�
																				   //Save->addTouchEventListener(CC_CALLBACK_2(StartScene::StartGameTouch, this));
	this->addChild(rootNode);

	return true;
}

void StartScene::StartGameTouch(Ref* pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::ENDED:
		auto director = Director::getInstance();
		auto scene = PlayLayer::createScene();
		//����л�����
		auto trasition = TransitionCrossFade::create(1.0, scene);
		director->replaceScene(trasition);
		break;
	}
}
