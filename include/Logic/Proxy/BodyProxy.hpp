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
//lua�еķ��� 
class LuaTimeEvecnt :public CTimeEvecnt {
public:
	LuaTimeEvecnt(sol::table table) :CTimeEvecnt()
	{
		LuaTable = table;
	}
	virtual void execute() {
		LuaTable["Execute"](LuaTable, this);//ִ�е�ǰ��lua�ص�
	}
public:
	sol::table LuaTable;//���ڴ洢lua���� 
}; 

class BodyProxy : public BaseProxy

{
private:
	b2World* m_World;//ÿ��������������һ�������ָ��,������ǰ���ɱ�������й���
public:
	BodyProxy();//��ǰ�Ĺ��캯��
public:
	std::unordered_map<ActorID, BodyData*> BodyMap;//���ڴ洢����� 
	std::unordered_map<BodyType, std::unordered_map<ActorID, BodyData*>> BodyManagerMap;//ĳһ���͵�body����
public:
	void SetWorld(b2World* world);//���õ�ǰ���������,֮�����еĸ��嶼����ӵ�����
	bool RegisterBody(BodyType type, b2Body* body);//ע�ᵱǰ��body����
	const std::unordered_map<ActorID, BodyData*>& GetBodyList(BodyType type);//��ȡ����ǰ�ĸ�����Ϣ�б�
	b2Body* GetBody(ActorID id);//��ȡ��һ������
	BodyType GetType(ActorID id);//��ȡ��һ������
	b2Body* CreateBody(BodyType type, std::string name, b2Vec2 postion = b2Vec2(0, 0));//����һ������
	void DeleteBody(b2Body* body);//ɾ��һ������
	void DeleteBody(ActorID ID);//ɾ��һ������ 
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