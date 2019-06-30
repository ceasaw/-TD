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
	//����
	
	Animation* createAnimation(std::string prefixName, int framesNum, float delay);
	virtual void changeDirection(float dt);
	//��ǰ��·��
	Node* currPoint();
	//��һ��·��
	Node* nextPoint();
	virtual void runFllowPoint();
	//�ƶ���·���㼯
	virtual void setPointsVector(Vector<Node*> points);
	virtual void createAndSetHpBar();
	virtual void enemyExpload() {};
private:
	Vector<Node*> pointsVector;

protected:
	//·����ı��
	int pointCounter;
	//����ͬ�Ķ���
	Animation *animationRight;
	Animation *animationLeft;
	Animation *animationExplode;
	CC_SYNTHESIZE(float, runSpeed, RunSpeed);
	CC_SYNTHESIZE(int, maxHp, MaxHp);
	CC_SYNTHESIZE(int, currHp, CurrHp);
	//�������İٷֱ�
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
