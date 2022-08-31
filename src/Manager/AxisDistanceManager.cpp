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
	Reset();//����һ������
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
//1����ǰ������Ӳ�
//2����ǰ�����Ӳ�
//3����,�Ӳ�ƫ�ƹ���,ֱ�����¼����ɫ�ӿڶ���
inline int ViewRange::JudgeActorViewAction()
{
	b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float xPercent = xMinus / (m_ObserverRange.x * 2);
	float yPercent = yMinus / (m_ObserverRange.y * 2);
	float finalPercent = std::max(xPercent, yPercent);  
	if (finalPercent <= 3) {//�Ӳ��������ƶ�����С�ڰٷ�֮3,��������
		m_ViewMinusPercent = finalPercent;
		return 1;
	}
	//���ڰٷ�֮3 С�ڵ��ڰٷ�֮30,���Ӳ����
	else if (finalPercent <= 30)
		return 2;
	else //���ڰٷ�֮30�ؼ���
		return 3;
}
//һ���˽���Ұ
bool ViewRange::EnterView(vector<ActorID> actorList) {
	for (auto item = m_ObserverMap.begin();item != m_ObserverMap.end(); item++)
		m_ObserverMap.insert(*item);
}
void ViewRange::LeaveView(ActorID actorID)
{
	m_ObserverMap.erase(actorID);
}

//һ���˽���Ұ
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
	InitIdelRangeMap(1000);//��������1000��
}

void AxisDistanceManager::InitIdelRangeMap(int size) {
	for (int i = 0; i < size;i++)
		m_IdleViewRnageMap.push_back(new ViewRangeTypeSet[2]);//������ô������ж���
}