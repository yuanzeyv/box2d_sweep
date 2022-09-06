#pragma once 
#include <iostream>
#include <stdint.h>  
#include "Manager/Define.h"
#include "Box2d/box2d.h"    
class BodyData
{ 
public: 
	BodyData(BodyType type, b2Body* body);
	void CalcBodyAABB();//计算AABB包围盒
	void CalcViewBody();//计算视野范围

	b2Body* GetBody() const;
	const b2AABB& GetAABB() const;//获取到碰撞AABB
	const b2AABB& GetViewAABB() const;//获取到视图AABB 
	inline const b2AABB& GetIgnoreAABB() const;//获取到视图AABB 
	void SetViewRange(float range);//设置视口范围
	//获取到当前的类型
	BodyType Type(); 
	~BodyData();//析构函数  
	ActorID ID();
protected:
	b2Body* Body; //当前监听的刚体信息
	BodyType BdType;//当前的道具类型 
	b2AABB BodyRange;//当前物体的AABB范围
	b2AABB BodyAABB;//当前物体的AABB范围
	b2AABB ViewAABB;//视口AABB 
	float ViewRange;//可以看到的距离
private:
	//获取到当前刚体的AABB范围
	void InitBodyAABB(); 
};

inline const b2AABB& BodyData::GetAABB() const
{
	return BodyAABB;
}
inline ActorID BodyData::ID()
{
	return Body->ID();
}
inline const b2AABB& BodyData::GetViewAABB() const
{
	return ViewAABB;
}
//获取到当前的类型
inline BodyType BodyData::Type()
{
	return BdType;
}
//获取到当前的类型
inline b2Body* BodyData::GetBody() const
{
	return Body;
}
//当前的忽略计算的范围

inline const b2AABB& BodyData::GetIgnoreAABB() const {//获取到视图AABB 
	return GetAABB();
}