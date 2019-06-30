#include "AppDelegate.h"
#include "base/CCVector.h"
#include "SimpleAudioEngine.h"
#include "TheStartScene.h"
USING_NS_CC;
using namespace std;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Sound/music.mp3");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Sound/game_music.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("Sound/dead.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("Sound/dead.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("Sound/dead1.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("Sound/shoot.wav");
	
	//SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/music.mp3",true);
	auto glview = director->getOpenGLView();
	if (!glview) {
		glview =
			GLViewImpl::createWithRect("TD Game", Rect(0, 0, 1536, 864));
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(static_cast<float>(1.0) / static_cast<float>(60.0));

	//·Ö±æÂÊÊÊÅä
	glview->setDesignResolutionSize(480.0f, 320.0f, ResolutionPolicy::FIXED_HEIGHT);
	//glview->setDesignResolutionSize(480.0f, 320.0f, ResolutionPolicy::NO_BORDER);
	//std::vector<std::string> searchPath;
	//searchPath.push_back("height_864");
	//CCFileUtils::getInstance()->setSearchPaths(searchPath);
	director->setContentScaleFactor(864.0f / 320.0f);


	FileUtils::getInstance()->addSearchPath("res");
	auto scene = PlayLayer::createScene();

	// run
	director->runWithScene(scene);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/music.mp3", true);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
