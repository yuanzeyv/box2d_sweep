#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>     
using std::unordered_map;
using std::unordered_set;
enum class ViewType {
	Null = 0,	//这是一个空单元
	Monster_Hero = 1 ,//怪物表
	Hero_Monster = 2 ,//英雄表 
	Hero_Hero = 3,//英雄到英雄
	Static_Hero = 4,//静态到英雄
	Hero_Static = 5,//英雄到静态
	Hero_Bullet = 6,//英雄到子弹
	Bullet_Hero = 7,//子弹到英雄
}; 
enum class ViewStatus {
	Visble = 0,
	Invisble = 1,
}; 
class DistanceManager
{
private:
	DistanceManager() {}//创建一个距离管理对象
public: 
	//当前所有的距离对象
	unordered_map<ActorID, BodyData*> DistanceObjMap;//视图类型 观察ID 观察状态 被观察ID 被观察距离   
	unordered_map<ViewType, unordered_map<ActorID, unordered_map<ViewStatus, unordered_map<ActorID,BodyData*>>>> DistanceMap;
	//计算身位
	unordered_map<ActorID, b2AABB> IgnoreCalcMap;//宽松身位
	unordered_set<ActorID> DelayedCalcMap;//延时计算  
public:
	bool RegisterBody(BodyData* body);
	void UnregisterBody(ActorID id);
	void UnregisterBody(BodyData* body);
	unordered_map<ActorID, BodyData*>& GetViewBody(ActorID id , ViewType type, ViewStatus view);
	void DistanceDump(ActorID id);
	//获取到视野范围内 最近的一个BodyData 
	b2Body* MinumumDistanceBody(ActorID id, ViewType checkType);
	//帧延时
	void DistanceCalcAdd(b2Body* body);
	void DistanceCalcAdd(BodyData* BodyData);
	void DistanceCalcRemove(b2Body* body);
	void DistanceCalcRemove(BodyData* BodyData);
	void DistanceCalcClear();
	void DistanceCalc(); 
	//打印当前可见对象
	void __Dump(ActorID id, ViewType type);
private:
	//重新计算
	inline void RecountDistance(b2Body* body);
	inline void RecountDistance(BodyData* body);
	//重新计算距离
	void __RecountDistance(BodyData* body, ViewType viewType, ViewType addType);
	//添加一个单元 将所有的 listType 下的单元加入到 viewType 下，并将BodyData单元加入到 addType下
	void __RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType);
	void __UnregisterBody(BodyData* body, ViewType viewType, ViewType addType);
public:
	static DistanceManager& Instance();
};
inline void DistanceManager::DistanceCalcAdd(b2Body* body) {
	if (!DistanceObjMap.count(body->ID())) 
		return;
	BodyData* calcBody = DistanceObjMap[body->ID()];
	DistanceCalcAdd(calcBody);
}
inline void DistanceManager::DistanceCalcAdd(BodyData* body) { 
	DelayedCalcMap.insert(body->ID());
}

inline void DistanceManager::DistanceCalcClear() {
	DelayedCalcMap.clear();
}
inline void DistanceManager::DistanceCalcRemove(b2Body* body) {
	BodyData* calcBody = DistanceObjMap[body->ID()];
	DistanceCalcRemove(calcBody);
}
inline void DistanceManager::DistanceCalcRemove(BodyData* BodyData) { 
	DelayedCalcMap.erase(BodyData->ID());
}

inline DistanceManager& DistanceManager::Instance() {
	static DistanceManager Instance;
	return Instance;
}

inline unordered_map<ActorID, BodyData*>& DistanceManager::GetViewBody(ActorID id, ViewType type, ViewStatus view)
{
	return DistanceMap[type][id][view];
}
inline void DistanceManager::RecountDistance(b2Body* body)
{
	RecountDistance(DistanceObjMap[body->ID()]);
}
