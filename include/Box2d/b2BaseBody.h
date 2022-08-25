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
	string BodyImage;//��ǰʹ�õ�ͼƬ
	int64_t BodyID;
	BodyType BdType;//��ǰ�ĵ������� 
	b2AABB* BodyRange;//��ǰ�����AABB��Χ
	b2AABB* BodyAABB;//��ǰ�����AABB��Χ
	b2AABB* ViewAABB;//�ӿ�AABB
public: 
	//��ȡ����ǰ�����AABB��Χ
	void InitBodyAABB();
	void CalcBodyAABB();
	void InitViewBody();
	void CalcViewBody();

	const b2AABB& GetAABB() const;//��ȡ����ײAABB
	const b2AABB& GetViewAABB() const;//��ȡ����ͼAABB
	//�������õ�ǰ�����λ��
	void SetPosition(float x,float y);
	//��ȡ����ǰbody��λ��
	b2Vec2 GetPosition();
	//��ȡ����ǰ����ת�Ƕ�
	float GetRotate();  
	//��ȡ����ǰ������
	BodyType Type();
	int64_t ID();  
	~BaseBody();//�������� 
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
//��ȡ����ǰ������
inline BodyType BaseBody::Type()
{
	return BdType;
}
inline int64_t BaseBody::ID()
{
	return BodyID;
}
  