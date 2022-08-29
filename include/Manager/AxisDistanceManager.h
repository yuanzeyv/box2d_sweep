#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <vector>     
//当前角色可以看到谁
//当前谁可以看到角色
using std::unordered_map;
using std::map;
using std::unordered_set;
using 
class AxisDistanceManager;
class ViewRange {
public: 
	BodyData*  m_Actor;//当前的角色
	unordered_map<ActorID, ViewRange*> m_ObserverMap;//可以看到的角色列表
	unordered_map<ActorID, ViewRange*> m_BeObserverMap;//被谁观察到
	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围
	b2Vec2 m_BodyPos;//角色记录在案的位置,用于查询角色
public:
	inline ViewRange(BodyData* bodyData) 
	{
		Reset();//清理一下数据
		m_Actor = bodyData;
	}  
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_BodyPos.Set(0, 0);
		m_ObserverMap.clear();
		m_BeObserverMap.clear();
	}
	//一组人进视野
	bool EnterView(vector<>){

	}
	~ViewRange()
	{ 
	}
};
class AxisDistanceManager
{
private:
	AxisDistanceManager() {}//创建一个距离管理对象
public: 
	//当前所有的距离对象
	map<float,unordered_map<ActorID,BodyData*>> XAxisBodyMap;//x坐标的角色记录
	map<float,unordered_map<ActorID,BodyData*>> YAxisBodyMap;//y坐标的角色记录

	map<ActorID, ViewRange*> ViewRangeMap;//角色对应的x轴与y轴 

	inline bool RegisterBody(ActorID id)
	{
		if (ViewRangeMap.count(id)) {
			return false; //已经注册过了
		}
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//通过身体控制单元获取到对应的身体
		ViewRangeMap[id] = new ViewRange(bodyData);
		return true;
	}
	void UnregisterBody(ActorID id)
	{
		if (ViewRangeMap.count(id)) 
			return ; //已经注册过了 

		delete ViewRangeMap[id];
		ViewRangeMap.erase(id);
	}

	//抢制让一个角色退出所有关联的角色视野
	void Force

	void UnregisterBody(BodyData* body);
	unordered_map<ActorID, BodyData*>& GetViewBody(ActorID id, ViewType type, ViewStatus view);
	//获取到视野范围内 最近的一个BodyData 
	b2Body* MinumumDistanceBody(ActorID id, ViewType checkType);
	//帧延时
	void DistanceCalcAdd(b2Body* body);
	void DistanceCalcAdd(BodyData* BodyData);
	void DistanceCalcRemove(b2Body* body);
	void DistanceCalcRemove(BodyData* BodyData);
	void DistanceCalcClear();
	void DistanceCalc(); 


	unordered_map<ViewType, unordered_map<ActorID, unordered_map<ViewStatus, unordered_map<ActorID,BodyData*>>>> DistanceMap;  
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
}; 
