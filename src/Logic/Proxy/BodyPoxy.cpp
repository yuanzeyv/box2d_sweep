#include "Logic/Proxy/BodyProxy.hpp"
#include "Manager/Base/BodyData.h" 
#include "Manager/DistanceManager.h"
#include "PySourceParse/PhysicsParse.h"
#include <vector>
#include <map> 
using namespace std;
BodyProxy::BodyProxy() :BaseProxy("BaseProxy") {
}

b2Body* BodyProxy::CreateBody(BodyType type, string name, b2Vec2 postion)//创建一个刚体
{
	b2Body* body = PhysicsParse::Instance().CreateBody(m_World, name, postion); //首先解析并在对应位置创建一个刚体
	RegisterBody(type, body);//将当前刚体注册到自己
	return body;//返回当前刚体信息
}
bool BodyProxy::RegisterBody(BodyType type, b2Body* body)
{
	auto bodyData = new BodyData(type, body);//首先创建一个数据存储单元 用于对body的初始化 
	ActorID ID = body->ID();
	BodyMap[ID] = bodyData;
	BodyManagerMap[type][ID] = bodyData;

	DistanceManager::Instance().RegisterBody(bodyData);//将当前刚体注册到距离对象中去(后期需要一个世界管理,会将这一个玩意加入到世界中去)
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
	m_World->DestroyBody(body);//删除当前刚体
	BodyMap.erase(ID);//删除对应刚体信息
	BodyManagerMap[type].erase(ID);//删除对应类型信息
	DistanceManager::Instance().UnregisterBody(ID);//距离单元删除

	delete bodyData;
}