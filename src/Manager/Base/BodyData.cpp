#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include "box2d/box2d.h"
#include "Manager/Define.h"

BodyData::BodyData(BodyType type,b2Body* body):ViewRange(5),Body(body),BdType(type)
{   
	InitBodyAABB();//初始化AABB盒子 
	CalcBodyAABB();//计算刚体的AABB
	CalcViewBody(); //计算刚体的可视范围
} 

//获取到当前刚体的AABB范围,调用一次之后便只管计算其距离了
void BodyData::InitBodyAABB()
{
	BodyRange.lowerBound = b2Vec2(9999, 9999);
	BodyRange.upperBound = b2Vec2(-9999, -9999);
	for (b2Fixture* f = Body->GetFixtureList(); f; f = f->GetNext())
	{
		int childCount = f->GetChildCount();
		for (int32 i = 0; i < childCount; ++i)
		{
			const b2AABB& aabb = f->GetAABB(i);
			BodyRange.Combine(aabb);
		}
	}
	//最后再减去当前角色的位置
	BodyRange.lowerBound -= Body->GetPosition();
	BodyRange.upperBound -= Body->GetPosition();
}  
void BodyData::CalcBodyAABB()
{  
	float radians = Body->GetAngle();
	float sinVal = sin(radians);
	float cosVal = cos(radians);
	float cosLowX = BodyRange.lowerBound.x * cosVal;
	float sinLowX = BodyRange.lowerBound.x * sinVal;
	float cosLowY = BodyRange.lowerBound.y * cosVal;
	float sinLowY = BodyRange.lowerBound.y * sinVal;
	float cosUpX = BodyRange.upperBound.x * cosVal;
	float sinUpX = BodyRange.upperBound.x * sinVal;
	float cosUpY = BodyRange.upperBound.y * cosVal;
	float sinUpY = BodyRange.upperBound.y * sinVal;
	//这是旋转后的包围盒
	b2Vec2 point[4];
	point[0].x = cosLowX - sinLowY;
	point[0].y = cosLowY + sinLowX;
	point[1].x = cosLowX - sinUpY;
	point[1].y = cosUpY + sinLowX;
	point[2].x = cosUpX - sinUpY;
	point[2].y = cosUpY + sinUpX;
	point[3].x = cosUpX - sinLowY;  
	point[3].y = cosLowY + sinUpX;

	BodyAABB.lowerBound.x = 99999;
	BodyAABB.lowerBound.y = 99999;
	BodyAABB.upperBound.x = -99999;
	BodyAABB.upperBound.y = -99999;
	for (int i = 0; i < 4; i++) {//循环16次 
		if (point[i].x > BodyAABB.upperBound.x)
			BodyAABB.upperBound.x = point[i].x;
		if (point[i].y > BodyAABB.upperBound.y)
			BodyAABB.upperBound.y = point[i].y;
		if (point[i].x < BodyAABB.lowerBound.x)
			BodyAABB.lowerBound.x = point[i].x;
		if (point[i].y < BodyAABB.lowerBound.y)
			BodyAABB.lowerBound.y = point[i].y;
	}
	const b2Vec2& pos = Body->GetPosition();
	BodyAABB.lowerBound.x += pos.x;
	BodyAABB.lowerBound.y += pos.y;
	BodyAABB.upperBound.x += pos.x;
	BodyAABB.upperBound.y += pos.y; 
}
void BodyData::CalcViewBody()
{
	auto bodyPos = Body->GetPosition();
	b2Vec2 viewPort(DistanceTrans(ViewRange), DistanceTrans(ViewRange));
	ViewAABB.lowerBound = bodyPos - viewPort;
	ViewAABB.upperBound = bodyPos + viewPort;
} 
//析构函数
BodyData::~BodyData()
{
}
//设置视口范围
void BodyData::SetViewRange(float range)
{
	ViewRange = range;
	CalcViewBody();
}