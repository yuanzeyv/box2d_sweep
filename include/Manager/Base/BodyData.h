#pragma once 
#include <iostream>
#include <stdint.h>  
#include "Box2d/box2d.h"   
//4����Ʒ����
enum class BodyType {
	NONE = 0,
	StaticBody = 1,
	PlayerBody = 2,
	MonsterBody = 3,
	BulletBody = 4,
};  
class BodyData
{ 
public: 
	BodyData(BodyType type, b2Body* body);
	void CalcBodyAABB();//����AABB��Χ��
	void CalcViewBody();//������Ұ��Χ

	b2Body* GetBody();
	const b2AABB& GetAABB() const;//��ȡ����ײAABB
	const b2AABB& GetViewAABB() const;//��ȡ����ͼAABB 
	inline const b2AABB& GetIgnoreAABB() const;//��ȡ����ͼAABB 
	void SetViewRange(float range);//�����ӿڷ�Χ
	//��ȡ����ǰ������
	BodyType Type(); 
	~BodyData();//��������  
	ActorID ID();
protected:
	b2Body* Body; //��ǰ�����ĸ�����Ϣ
	BodyType BdType;//��ǰ�ĵ������� 
	b2AABB BodyRange;//��ǰ�����AABB��Χ
	b2AABB BodyAABB;//��ǰ�����AABB��Χ
	b2AABB ViewAABB;//�ӿ�AABB 
	float ViewRange;//���Կ����ľ���
private:
	//��ȡ����ǰ�����AABB��Χ
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
//��ȡ����ǰ������
inline BodyType BodyData::Type()
{
	return BdType;
}
//��ȡ����ǰ������
inline b2Body* BodyData::GetBody()
{
	return Body;
}
//��ǰ�ĺ��Լ���ķ�Χ

inline const b2AABB& BodyData::GetIgnoreAABB() const {//��ȡ����ͼAABB 
	return GetAABB();
}