#pragma once
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h" 
#include "Manager/BodyManager.h"
#include "Manager/DistanceManager.h"
#include "Manager/AxisDistanceManager.h"
#include <map> 
#include <vector>   
using namespace std;   

ViewRange::ViewRange(BodyData* bodyData)
{
	Reset();//清理一下数据
	m_Actor = bodyData;
}
void ViewRange::Reset()
{
	m_Actor = NULL;
	m_ObserverRange.Set(0.5, 0.5); 
	m_BodyPos.Set(MAX_DISTANCE, MAX_DISTANCE);
	m_ObserverMap.clear();
	m_BeObserverMap.clear();
	m_ViewMinusPercent = 0;
} 

inline void ViewRange::RecalcViewRange()
{
	const b2AABB & bodyAABB = m_Actor->GetAABB();
	m_BodyAABB.lowerBound.Set(m_Actor->GetAABB().lowerBound);
	m_BodyAABB.upperBound.Set(m_Actor->GetAABB().upperBound); 
}

inline const b2Vec2& ViewRange::GetViewRange()
{
	return m_ObserverRange;
}
//1代表当前仅添加视差
//2代表当前计算视差
//3代表,视差偏移过大,直接重新计算角色视口对象
inline int ViewRange::JudgeActorViewAction()
{
	b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();//获取到当前的位置 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float xPercent = xMinus / (m_ObserverRange.x * 2);
	float yPercent = yMinus / (m_ObserverRange.y * 2);
	float finalPercent = std::max(xPercent, yPercent);  
	if (finalPercent <= 3) {//视差任意轴移动距离小于百分之3,不做计算
		m_ViewMinusPercent = finalPercent;
		return 1;
	}
	//大于百分之3 小于等于百分之30,做视差计算
	else if (finalPercent <= 30)
		return 2;
	else //大于百分之30重计算
		return 3;
}
//一组人进视野
bool ViewRange::EnterView(vector<ActorID> actorList) {
	for (auto item = m_ObserverMap.begin();item != m_ObserverMap.end(); item++)
		m_ObserverMap.insert(*item);
}
void ViewRange::LeaveView(ActorID actorID)
{
	m_ObserverMap.erase(actorID);
}

//一组人进视野
bool ViewRange::EnterOtherView(ActorID actorID) {
	m_BeObserverMap.insert(actorID);
}
void ViewRange::LeaveOtherView(ActorID actorID) {
	m_BeObserverMap.erase(actorID);
}
void ViewRange::SetAxisPos(const b2Vec2& pos)
{
	m_BodyPos.Set(pos.x, pos.y);
}
const b2Vec2& ViewRange::GetAxisPos()
{
	return this->m_BodyPos;
}
ViewRange::~ViewRange()
{
} 

AxisDistanceManager::AxisDistanceManager()
{
	InitIdelRangeMap(1000);//上来分配1000个
}

void AxisDistanceManager::InitIdelRangeMap(int size) {
	for (int i = 0; i < size;i++)
		m_IdleViewRnageMap.push_back(new ViewRangeTypeSet[2]);//创建这么多个空闲对象
}