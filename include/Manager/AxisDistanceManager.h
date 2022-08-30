#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <list>     
#include <stack>
//当前角色可以看到谁
//当前谁可以看到角色
using std::unordered_map;
using std::map;
using std::unordered_set;
using std::list;
using std::stack;
class AxisDistanceManager; 
class ViewRange {
public: 
	BodyData*  m_Actor;//当前的角色
	unordered_set<ActorID> m_ObserverMap;//可以看到的角色列表
	unordered_set<ActorID> m_BeObserverMap;//被谁观察到

	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围 
	b2AABB m_BodyViewRange;//记录角色的视野范围

	b2Vec2 m_BodyPos;//记录角色最后一次记录的位置
	//只有视差范围很小时,才会进行视差检查,如果视差已经大于视口 那么将会重新计算
	float m_ViewMinusPercent;//可积累视差,积累到一定距离时,将进行一次距离计算,且本次距离计算为局部距离计算

	DirectionType m_FourPointDir[4];//标记角色四个点在什么位置 
public:
	inline ViewRange(BodyData* bodyData) 
	{
		Reset();//清理一下数据
		m_Actor = bodyData;
	}  
	//1代表当前仅添加视差
	//2代表当前计算视差
	//3代表,视差偏移过大,直接重新计算角色视口对象
	inline int CalcActorViewRange()
	{
		b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();
		float xMinus = bodyPos.x - m_BodyPos.x;
		float yMinus = bodyPos.x - m_BodyPos.x;

		//视差任意轴移动距离小于百分之3,不做计算
		//大于百分之3 小于等于百分之30,做视差计算
		//大于百分之30重计算
	}
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_BodyPos.Set(MAX_DISTANCE, MAX_DISTANCE);
		m_ObserverMap.clear();
		m_BeObserverMap.clear();
		m_ViewMinusPercent = 0;
	}

	//一组人进视野
	bool EnterView(vector<ActorID> actorList){
		for (auto item= m_ObserverMap.begin() ;item != m_ObserverMap.end(); item++)
			m_ObserverMap.insert(*item);
	}
	void LeaveView(ActorID actorID)
	{
		m_ObserverMap.erase(actorID);
	}

	//一组人进视野
	bool EnterOtherView(ActorID actorID) { 
		m_BeObserverMap.insert(actorID);
	}
	void LeaveOtherView(ActorID actorID){
		m_BeObserverMap.erase(actorID);
	}
	void SetAxisPos(const b2Vec2& pos)
	{
		m_AxisPos.Set(pos.x, pos.y);
	}
	const b2Vec2& GetAxisPos()
	{
		return this->m_AxisPos;
	}
	~ViewRange()
	{ 
	}
};
class AxisDistanceManager
{ 
public: 
	//当前所有的距离对象
	map<float, unordered_map<ActorID, PointDir*>> m_XAxisBodyMap;//x坐标的角色记录 (记录顶点的左 右)
	map<float, unordered_map<ActorID, PointDir*>> m_YAxisBodyMap;//y坐标的角色记录 (记录顶点的上 下) 

	unordered_map<ActorID, ViewRange*> m_ViewRangeMap;//角色对应的范围信息
	AxisDistanceManager()
	{ 
	}
	void InitDirList(int size){   
	}

	inline bool RegisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id)) {
			return false; //已经注册过了
		}
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//通过身体控制单元获取到对应的身体
		m_ViewRangeMap[id] = new ViewRange(bodyData);
		return true;
	}
	void UnregisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id))
			return ; //已经注册过了 
		ForceCleanElseView(id);
		delete m_ViewRangeMap[id];
		m_ViewRangeMap.erase(id);
	}
	//注册角色AABB到队列中去
	void RegisterActorAABB(){

	}
	//删除角色AABB到队列
	void 


	//抢制让一个角色退出所有关联的角色视野
	void ForceCleanElseView(ActorID actorID)
	{
		if (!m_ViewRangeMap.count(actorID))
			return;
		ViewRange* rangeObj = m_ViewRangeMap[actorID];//获取到range对象
		ViewRange* observerViewObj = NULL;//获取到range对象
		//获取到对象所观察到的每个人
		for (auto item = rangeObj->m_ObserverMap.begin(); item != rangeObj->m_ObserverMap.end(); item++)
		{
			if (!m_ViewRangeMap.count(*item)) {
				printf("遇到了点小问题");
				continue;//继续删除下一个
			} 
			observerViewObj = m_ViewRangeMap[*item];
			observerViewObj->LeaveOtherView(*item);//强制删除的玩家离开了此玩家的视野
		} 
		//删除所有它看到的人
		ViewRange* beObserverViewObj = NULL;
		for (auto item = rangeObj->m_BeObserverMap.begin(); item != rangeObj->m_BeObserverMap.end(); item++)
		{
			if (!m_ViewRangeMap.count(*item)) {
				printf("遇到了点小问题");
				continue;//继续删除下一个
			}
			observerViewObj = m_ViewRangeMap[*item];
			observerViewObj->LeaveView(*item);//强制删除的玩家离开了此玩家的视野
		} 
	} 
	//注册距离
	void ReCalcPosition(ActorID actorID,const b2Vec2& origin, const b2Vec2& now)
	{
		if (m_XAxisBodyMap.count(origin.x)) { 
			if (m_XAxisBodyMap[origin.x].count(actorID))
				m_XAxisBodyMap[origin.x].erase(actorID);
		}
		if (m_YAxisBodyMap.count(origin.y)) {
			if (m_YAxisBodyMap[origin.y].count(actorID))
				m_YAxisBodyMap[origin.y].erase(actorID);
		}
		//再次将当前距离注册进去
		if (m_XAxisBodyMap.count(now.x) == 0) {
			m_XAxisBodyMap[now.x];//创建一下
		}
		if (m_YAxisBodyMap.count(now.y) == 0) {
			m_YAxisBodyMap[now.y];//创建一下
		}
		m_XAxisBodyMap[now.x].insert(actorID);
		m_YAxisBodyMap[now.y].insert(actorID); 
	}
	//计算距离
	void CalcDistance(ActorID actorID)
	{
		if (!m_ViewRangeMap.count(actorID))
			return;
		ViewRange* rangeObj = m_ViewRangeMap[actorID];//获取到range对象
		BodyData* bodyData = rangeObj->m_Actor;	//获取到当前的身体信息
		const b2Vec2& axisPos = bodyData->GetBody()->GetPosition();
		const b2Vec2& b2Pos = bodyData->GetBody()->GetPosition();
		//首先删除数组中的距离
		ReCalcPosition(actorID,axisPos, b2Pos);//计算距离
		rangeObj->SetAxisPos(b2Pos);//重新数组方向

	}
	//获取到当前距离内的所有成员
	void GetRangeDistanceObjs(unordered_set<ActorID>& actors,const b2Vec2& range,const b2Vec2& pos)
	{
		float minX = pos.x - range.x, maxX = pos.x + range.x;//当前的最小范围
		float minY = pos.y - range.y, maxY = pos.y + range.y;//当前的最大范围
		unordered_set<ActorID> compareXSet;  
		ptrdiff_t distanceX = std::distance(m_XAxisBodyMap.find(pos.x), m_XAxisBodyMap.end());
		ptrdiff_t distanceY = std::distance(m_YAxisBodyMap.find(pos.x), m_YAxisBodyMap.end());
		//首先获取到反向迭代器
		for (auto item = m_XAxisBodyMap.rbegin() + (m_XAxisBodyMap.size() - distanceX); item == m_XAxisBodyMap.rend(); item++)
		{
			if (item->first > maxX)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				compareXSet.insert(*actorSet);
		}
		//首先获取到反向迭代器
		for (auto item = m_XAxisBodyMap.find(pos.x); item == m_XAxisBodyMap.end(); item++)
		{
			if (item->first < minX)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				compareXSet.insert(*actorSet);
		} 
		//首先获取到反向迭代器
		for (auto item = m_YAxisBodyMap.rbegin() + (m_YAxisBodyMap.size() - distanceY); item == m_YAxisBodyMap.rend(); item++)
		{
			if (item->first > maxY)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				if (compareXSet.count(*actorSet))
					actors.insert(*actorSet); 
		}
		//首先获取到反向迭代器
		for (auto item = m_YAxisBodyMap.find(pos.x); item == m_YAxisBodyMap.end(); item++)
		{
			if (item->first < minY)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				if (compareXSet.count(*actorSet))
					actors.insert(*actorSet);
		}
	} 
}; 
