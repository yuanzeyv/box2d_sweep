#pragma once
#include "Manager/Base/BodyData.h" 
#include <unordered_map> 
#include <unordered_set>  
class BodyManager
{
private:
	b2World* World;//每个身体管理对象都有一个世界的指针,声明当前是由本世界进行管理
private:
	BodyManager();//当前的构造函数
public:
	std::unordered_map<ActorID, BodyData*> BodyMap;//用于存储对象的 
	std::unordered_map<BodyType, std::unordered_map<ActorID, BodyData*>> BodyManagerMap;//某一类型的body队列
public:
	void SetWorld(b2World* world);//设置当前的世界对象,之后所有的刚体都是添加到这里
	bool RegisterBody(BodyType type, b2Body* body);//注册当前的body对象
	const std::unordered_map<ActorID, BodyData*>& GetBodyList(BodyType type);//获取到当前的刚体信息列表
	inline b2Body* GetBody(ActorID id);//获取到一个刚体
	inline BodyData* GetBodyData(ActorID id);
	BodyType GetType(ActorID id);//获取到一个类型
	b2Body* CreateBody(BodyType type, std::string name,b2Vec2 postion = b2Vec2(0,0));//创建一个刚体
	void DeleteBody(b2Body* body);//删除一个刚体
	void DeleteBody(ActorID ID);//删除一个刚体
	static BodyManager& Instance();//获取到单利
};
inline BodyData* BodyManager::GetBodyData(ActorID id)
{
	if (BodyMap.count(id) == 0)
		return NULL;
	return BodyMap[id];
}
inline b2Body* BodyManager::GetBody (ActorID id)
{
	if (BodyMap.count(id) == 0 )
		return NULL;
	return BodyMap[id]->GetBody();
}
inline BodyType BodyManager::GetType(ActorID id)
{
	if (BodyMap.count(id) == 0)
		return BodyType::BODY_TYPE_NULL;
	return BodyMap[id]->Type();
}
inline void BodyManager::SetWorld(b2World* world)
{
	World = world;
}
inline BodyManager& BodyManager::Instance() {
	static BodyManager Instance;
	return Instance;
}
