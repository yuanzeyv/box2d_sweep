#pragma once 
#include <iostream>
#include <stdint.h> 
#include "box2d/b2_body.h"
using namespace std; 
class b2AABB;
class b2Vec2;
class b2Shape; 
class b2World;
class b2FixtureDef;
class b2Transform;
enum b2BodyType; 
enum class BodyType
{
	NONE = 0,
	StaticBody = 1,
	PlayerBody = 2,
	MonsterBody = 3,
	BulletBody = 4,
};

class BaseBody:public b2Body
{  
protected: 
	string BodyImage;//当前使用的图片
	int64_t BodyID;
	BodyType BdType;//当前的道具类型 
	b2AABB* BodyRange;//当前物体的AABB范围
	b2AABB* BodyAABB;//当前物体的AABB范围
	b2AABB* ViewAABB;//视口AABB
public: 
	//获取到当前刚体的AABB范围
	void InitBodyAABB();
	void CalcBodyAABB();
	void InitViewBody();
	void CalcViewBody();

	const b2AABB& GetAABB() const;//获取到碰撞AABB
	const b2AABB& GetViewAABB() const;//获取到视图AABB
	//重新设置当前刚体的位置
	void SetPosition(float x,float y);
	//获取到当前body的位置
	b2Vec2 GetPosition();
	//获取到当前的旋转角度
	float GetRotate();  
	//获取到当前的类型
	BodyType Type();
	int64_t ID();  
	~BaseBody();//析构函数 
private:
	BaseBody(string image, b2World* world);
};

inline const b2AABB& BaseBody::GetAABB() const
{
	return *BodyAABB;
}
inline const b2AABB& BaseBody::GetViewAABB() const
{
	return *ViewAABB;
}
//获取到当前的类型
inline BodyType BaseBody::Type()
{
	return BdType;
}
inline int64_t BaseBody::ID()
{
	return BodyID;
}
  