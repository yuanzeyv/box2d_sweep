#include "Logic/Proxy/BodyProxy.hpp"
#include "Manager/Base/BodyData.h" 
#include "Manager/DistanceManager.h"
#include "PySourceParse/PhysicsParse.h"
#include <vector>
#include <map> 
using namespace std;
BodyProxy::BodyProxy() :BaseProxy("BaseProxy") {
}

b2Body* BodyProxy::CreateBody(BodyType type, string name, b2Vec2 postion)//����һ������
{
	b2Body* body = PhysicsParse::Instance().CreateBody(m_World, name, postion); //���Ƚ������ڶ�Ӧλ�ô���һ������
	RegisterBody(type, body);//����ǰ����ע�ᵽ�Լ�
	return body;//���ص�ǰ������Ϣ
}
bool BodyProxy::RegisterBody(BodyType type, b2Body* body)
{
	auto bodyData = new BodyData(type, body);//���ȴ���һ�����ݴ洢��Ԫ ���ڶ�body�ĳ�ʼ�� 
	ActorID ID = body->ID();
	BodyMap[ID] = bodyData;
	BodyManagerMap[type][ID] = bodyData;

	DistanceManager::Instance().RegisterBody(bodyData);//����ǰ����ע�ᵽ���������ȥ(������Ҫһ���������,�Ὣ��һ��������뵽������ȥ)
	return true;
}
const unordered_map<ActorID, BodyData*>& BodyProxy::GetBodyList(BodyType type)
{
	return BodyManagerMap[type];
}
void BodyProxy::DeleteBody(b2Body* body)
{
	DeleteBody(body->ID());
}
void BodyProxy::DeleteBody(ActorID ID)
{
	if (BodyMap.count(ID) == 0)
		return;
	BodyData* bodyData = BodyMap[ID];
	BodyType type = bodyData->Type();
	b2Body* body = bodyData->GetBody();
	m_World->DestroyBody(body);//ɾ����ǰ����
	BodyMap.erase(ID);//ɾ����Ӧ������Ϣ
	BodyManagerMap[type].erase(ID);//ɾ����Ӧ������Ϣ
	DistanceManager::Instance().UnregisterBody(ID);//���뵥Ԫɾ��

	delete bodyData;
}