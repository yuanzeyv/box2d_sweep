#pragma once
#include "Manager/Base/BodyData.h"   
#include <unordered_map> 
#include <unordered_set> 

#include "PureMVC/PureMVC.hpp" 
#include "Logic/Proxy/BaseProxy.hpp"
#include "Library/TimeWheel/timer-wheel.h" 
#include "Library/sol/sol.hpp" 
class CTimeEvecnt :public TimerEventInterface {
public:
	CTimeEvecnt() :TimerEventInterface() {
	}
	void AddTime(TimerWheel& timers, Tick millisecond) {
		timers.schedule(this, millisecond);
	}
	virtual void execute() {

	}
};
//lua中的方法 
class LuaTimeEvecnt :public CTimeEvecnt {
public:
	LuaTimeEvecnt(sol::table table) :CTimeEvecnt()
	{
		LuaTable = table;
	}
	virtual void execute() {
		LuaTable["Execute"](LuaTable, this);//执行当前的lua回调
	}
public:
	sol::table LuaTable;//用于存储lua方法 
}; 

class BodyProxy : public BaseProxy

{
private:
	b2World* m_World;//每个身体管理对象都有一个世界的指针,声明当前是由本世界进行管理
public:
	BodyProxy();//当前的构造函数
public:
	std::unordered_map<ActorID, BodyData*> BodyMap;//用于存储对象的 
	std::unordered_map<BodyType, std::unordered_map<ActorID, BodyData*>> BodyManagerMap;//某一类型的body队列
public:
	void SetWorld(b2World* world);//设置当前的世界对象,之后所有的刚体都是添加到这里
	bool RegisterBody(BodyType type, b2Body* body);//注册当前的body对象
	const std::unordered_map<ActorID, BodyData*>& GetBodyList(BodyType type);//获取到当前的刚体信息列表
	b2Body* GetBody(ActorID id);//获取到一个刚体
	BodyType GetType(ActorID id);//获取到一个类型
	b2Body* CreateBody(BodyType type, std::string name, b2Vec2 postion = b2Vec2(0, 0));//创建一个刚体
	void DeleteBody(b2Body* body);//删除一个刚体
	void DeleteBody(ActorID ID);//删除一个刚体 
};     
inline b2Body* BodyProxy::GetBody(ActorID id)
{
	if (BodyMap.count(id) == 0)
		return NULL;
	return BodyMap[id]->GetBody();
}
inline BodyType BodyProxy::GetType(ActorID id)
{
	if (BodyMap.count(id) == 0)
		return BodyType::BODY_TYPE_NULL;
	return BodyMap[id]->Type();
}
inline void BodyProxy::SetWorld(b2World* world)
{
	m_World = world;
} 