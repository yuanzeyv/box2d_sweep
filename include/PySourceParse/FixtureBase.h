

#ifndef _FIXTURE_BASE_H_
#define _FIXTURE_BASE_H_
#include "PySourceParse/ShapeBase.h"  
#include "box2d/box2d.h"
#include <vector> 
#include <string>
#include <iostream>  
class FixtureBase {
public:
	std::string Name;
	bool IsSensor;//是否为传感器
	float Density;//夹具质量
	float Friction;//夹具摩擦
	float Restitution;//夹具弹性
	uint16 CategoryBits;//夹具屏蔽字
	uint16 GroupIndex;
	uint16 MaskBits;
	double Format;//获取到格式
	double Ratio;//获取到身体缩放  
	std::vector<ShapeBase*> ShapeVec;
public:
	bool InsertShape(ShapeBase* shape)
	{
		ShapeVec.push_back(shape);
		shape->InitShape();
		return true;
	} 
	std::vector<b2FixtureDef> GetFixtureDefines()
	{  
		std::vector<b2FixtureDef> FixtureList;
		for (int i = 0; i < ShapeVec.size(); i++)
		{
			b2FixtureDef tempDef;
			tempDef.shape = ShapeVec[i]->Shape;
			tempDef.density = Density;
			tempDef.friction = Friction;
			tempDef.restitution = Restitution; 
			tempDef.filter.categoryBits = CategoryBits;
			tempDef.filter.groupIndex = GroupIndex;
			tempDef.filter.maskBits = MaskBits;
			FixtureList.push_back(tempDef); 
		}
		return FixtureList;
	} 
}; 
#endif //  _FIXTURE_BASE_H_
