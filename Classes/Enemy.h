#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "cocos2d.h"
USING_NS_CC;

typedef enum
{
	PRIATE,
	THIEF,
	BANDIT,
	CRAZYDRAGON
}EnemyType;

class Enemy : public Sprite
{
public:
	Enemy();
	//Enemy(Enemy*& enemy);
	virtual bool init();
	CREATE_FUNC(Enemy);
	//动画
	
	Animation* createAnimation(std::string prefixName, int framesNum, float delay);
	virtual void changeDirection(float dt);
	//当前的路点
	Node* currPoint();
	//下一个路点
	Node* nextPoint();
	virtual void runFllowPoint();
	//移动的路径点集
	virtual void setPointsVector(Vector<Node*> points);
	virtual void createAndSetHpBar();
	virtual void enemyExpload() {};
private:
	Vector<Node*> pointsVector;

protected:
	//路径点的标号
	int pointCounter;
	//朝向不同的动画
	Animation *animationRight;
	Animation *animationLeft;
	Animation *animationExplode;
	CC_SYNTHESIZE(float, runSpeed, RunSpeed);
	CC_SYNTHESIZE(int, maxHp, MaxHp);
	CC_SYNTHESIZE(int, currHp, CurrHp);
	//进度条的百分比
	CC_SYNTHESIZE(float, hpPercentage, HpPercentage);
	CC_SYNTHESIZE_READONLY(ProgressTimer*, hpBar, HpBar);
	CC_SYNTHESIZE(bool, enemySuccessful, EnemySuccessful);
	CC_SYNTHESIZE(int, isDead, IsDead);
	CC_SYNTHESIZE(EnemyType, enemyType, EnemyType);
	//virtual void enemyExpload();
	Sprite* sprite;
	Sprite* hpBgSprite;
};

#endif // !_ENEMY_H_
