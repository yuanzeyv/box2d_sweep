

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
	bool IsSensor;//�Ƿ�Ϊ������
	float Density;//�о�����
	float Friction;//�о�Ħ��
	float Restitution;//�оߵ���
	uint16 CategoryBits;//�о�������
	uint16 GroupIndex;
	uint16 MaskBits;
	double Format;//��ȡ����ʽ
	double Ratio;//��ȡ����������  
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
