#pragma once
#ifndef _LOAD_LEVEL_INFO_H_
#define _LOAD_LEVEL_INFO_H_
#include "cocos2d.h"
USING_NS_CC;

class LoadLevelinfo : public Ref
{
public:

	~LoadLevelinfo();
	static LoadLevelinfo * createLoadLevelinfo(const std::string& plistpath);

	bool initPlist(const std::string& plistpath);
	void readLevelInfo();
	void clearAll();

private:
	ValueMap resources;
	ValueMap levelInfo;
};

#endif // !_LOAD_LEVEL_INFO_H_

