#include "LoadLevelInfo.h"
#include "GameManager.h"
#include "GroupEnemy.h"

LoadLevelinfo::~LoadLevelinfo()
{
	clearAll();
}

LoadLevelinfo *LoadLevelinfo::createLoadLevelinfo(const std::string& plistpath)
{
	LoadLevelinfo *pRet = new LoadLevelinfo();
	if (pRet && pRet->initPlist(plistpath))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool LoadLevelinfo::initPlist(const std::string& plistpath)
{
	bool bRet = false;
	do
	{
		// 1 plistpath是.plist文件的相对路径，这里通过FileUtils类获得给定文件名的完整路径，
		//   再把该文件中的内容(类型为Dictionary)加载到ValueMap的对象中保存。
		std::string fullPath = FileUtils::getInstance()->fullPathForFilename(plistpath);
		// 仅仅加载dictionary的内容
		ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
		// 2  放到Map中即可用Map的方法读取键为”id"的值是多少，
		//    分别读取dict对象中键为”resources"，”levelInfo"的值，
		//    它们的类型依旧是Dictionary，所以依旧将其内容转换到ValueMap对象中保存。  
		resources = dict["resources"].asValueMap();
		levelInfo = dict["levelInfo"].asValueMap();
		bRet = true;
	} while (0);
	return bRet;
}

void LoadLevelinfo::clearAll()
{
	resources.clear();
	levelInfo.clear();
}

void LoadLevelinfo::readLevelInfo()
{
	GameManager *instance = GameManager::getInstance();
	// 3 根据plist文件的属性和层次特征，
	//   一层一层的遍历获得相应类型的键值，再把它们存储到GameManager中。
	auto money = levelInfo["money"].asFloat();
	instance->setMoney(money);
	auto currlevel = levelInfo["currlevel"].asString();
	instance->setCurrLevelFile(currlevel);
	auto nextlevel = levelInfo["nextlevel"].asString();
	instance->setNextLevelFile(nextlevel);

	ValueMap& groupDict = levelInfo["group"].asValueMap();

	auto groupTotle = groupDict.size();
	instance->setGroupNum(groupTotle);

	for (auto iter = groupDict.begin(); iter != groupDict.end(); ++iter)
	{
		ValueMap& group = iter->second.asValueMap();
		std::string spriteFrameName = iter->first;
		auto type1Num = group["type1Num"].asInt();
		auto type2Num = group["type2Num"].asInt();
		auto type3Num = group["type3Num"].asInt();
		auto type1Hp = group["type1Hp"].asInt();
		auto type2Hp = group["type2Hp"].asInt();
		auto type3Hp = group["type3Hp"].asInt();

		GroupEnemy* groupEnemy = GroupEnemy::create()->initGroupEnemy(type1Num, type1Hp, type2Num, type2Hp, type3Num, type3Hp);
		instance->groupVector.pushBack(groupEnemy);
	}

	auto curMapName = resources["map"].asString();
	instance->setCurMapName(curMapName);
	auto curBgName = resources["image"].asString();
	instance->setCurBgName(curBgName);
}