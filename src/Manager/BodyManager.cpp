#pragma once 
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h"
#include "Manager/AxisDistanceManager.h"
#include "PySourceParse/PhysicsParse.h"
#include <vector>
#include <map> 
using namespace std;
BodyManager::BodyManager():World(NULL){
}

b2Body* BodyManager::CreateBody(BodyType type, string name, b2Vec2 postion)//����һ������
{
	b2Body* body = PhysicsParse::Instance().CreateBody(World,name, postion); //���Ƚ������ڶ�Ӧλ�ô���һ������
	Instance().RegisterBody(type, body);//����ǰ����ע�ᵽ�Լ�
	return body;//���ص�ǰ������Ϣ
}
bool BodyManager::RegisterBody(BodyType type, b2Body* body)
{
	auto bodyData = new BodyData(type, body);//���ȴ���һ�����ݴ洢��Ԫ ���ڶ�body�ĳ�ʼ�� 
	ActorID ID = body->ID();
	BodyMap[ID] = bodyData; 
	BodyManagerMap[type][ID] = bodyData;

	AxisDistanceManager::Instance().RegisterBody(bodyData);//����ǰ����ע�ᵽ���������ȥ(������Ҫһ���������,�Ὣ��һ��������뵽������ȥ)
	return true;
}
const unordered_map<ActorID, BodyData*>& BodyManager::GetBodyList(BodyType type)
{
	return BodyManagerMap[type];
} 
void BodyManager::DeleteBody(b2Body* body)
{
	DeleteBody(body->ID()); 
}
void BodyManager::DeleteBody(ActorID ID)
{
	if (BodyMap.count(ID) == 0)
		return; 
	BodyData* bodyData = BodyMap[ID];
	BodyType type = bodyData->Type();
	b2Body* body = bodyData->GetBody(); 
	World->DestroyBody(body);//ɾ����ǰ����
	BodyMap.erase(ID);//ɾ����Ӧ������Ϣ
	BodyManagerMap[type].erase(ID);//ɾ����Ӧ������Ϣ
	AxisDistanceManager::Instance().UnregisterBody(ID);//���뵥Ԫɾ��

	delete bodyData;
}