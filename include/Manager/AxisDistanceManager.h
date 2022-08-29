#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <vector>     
//��ǰ��ɫ���Կ���˭
//��ǰ˭���Կ�����ɫ
using std::unordered_map;
using std::map;
using std::unordered_set;
using std::vector;
class AxisDistanceManager;
class ViewRange {
public: 
	BodyData*  m_Actor;//��ǰ�Ľ�ɫ
	unordered_set<ActorID> m_ObserverMap;//���Կ����Ľ�ɫ�б�
	unordered_set<ActorID> m_BeObserverMap;//��˭�۲쵽
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ
	b2Vec2 m_AxisPos;//��ɫ��¼�ڰ���λ��,���ڲ�ѯ��ɫ
public:
	inline ViewRange(BodyData* bodyData) 
	{
		Reset();//����һ������
		m_Actor = bodyData;
	}  
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_AxisPos.Set(MAX_DISTANCE, MAX_DISTANCE);
		m_ObserverMap.clear();
		m_BeObserverMap.clear();
	}
	//һ���˽���Ұ
	bool EnterView(vector<ActorID> actorList){
		for (auto item= m_ObserverMap.begin() ;item != m_ObserverMap.end(); item++)
			m_ObserverMap.insert(*item);
	}
	void LeaveView(ActorID actorID)
	{
		m_ObserverMap.erase(actorID);
	}

	//һ���˽���Ұ
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
private:
	AxisDistanceManager() {}//����һ������������
public: 
	//��ǰ���еľ������
	map<float,unordered_set<ActorID>> m_XAxisBodyMap;//x����Ľ�ɫ��¼
	map<float,unordered_set<ActorID>> m_YAxisBodyMap;//y����Ľ�ɫ��¼

	map<ActorID, ViewRange*> m_ViewRangeMap;//��ɫ��Ӧ��x����y�� 

	inline bool RegisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id)) {
			return false; //�Ѿ�ע�����
		}
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//ͨ��������Ƶ�Ԫ��ȡ����Ӧ������
		m_ViewRangeMap[id] = new ViewRange(bodyData);
		return true;
	}
	void UnregisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id))
			return ; //�Ѿ�ע����� 
		ForceCleanElseView(id);
		delete m_ViewRangeMap[id];
		m_ViewRangeMap.erase(id);
	}

	//������һ����ɫ�˳����й����Ľ�ɫ��Ұ
	void ForceCleanElseView(ActorID actorID)
	{
		if (!m_ViewRangeMap.count(actorID))
			return;
		ViewRange* rangeObj = m_ViewRangeMap[actorID];//��ȡ��range����
		ViewRange* observerViewObj = NULL;//��ȡ��range����
		//��ȡ���������۲쵽��ÿ����
		for (auto item = rangeObj->m_ObserverMap.begin(); item != rangeObj->m_ObserverMap.end(); item++)
		{
			if (!m_ViewRangeMap.count(*item)) {
				printf("�����˵�С����");
				continue;//����ɾ����һ��
			} 
			observerViewObj = m_ViewRangeMap[*item];
			observerViewObj->LeaveOtherView(*item);//ǿ��ɾ��������뿪�˴���ҵ���Ұ
		} 
		//ɾ����������������
		ViewRange* beObserverViewObj = NULL;
		for (auto item = rangeObj->m_BeObserverMap.begin(); item != rangeObj->m_BeObserverMap.end(); item++)
		{
			if (!m_ViewRangeMap.count(*item)) {
				printf("�����˵�С����");
				continue;//����ɾ����һ��
			}
			observerViewObj = m_ViewRangeMap[*item];
			observerViewObj->LeaveView(*item);//ǿ��ɾ��������뿪�˴���ҵ���Ұ
		} 
	} 
	//ע�����
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
		//�ٴν���ǰ����ע���ȥ
		if (m_XAxisBodyMap.count(now.x) == 0) {
			m_XAxisBodyMap[now.x];//����һ��
		}
		if (m_YAxisBodyMap.count(now.y) == 0) {
			m_YAxisBodyMap[now.y];//����һ��
		}
		m_XAxisBodyMap[now.x].insert(actorID);
		m_YAxisBodyMap[now.y].insert(actorID); 
	}
	//�������
	void CalcDistance(ActorID actorID)
	{
		if (!m_ViewRangeMap.count(actorID))
			return;
		ViewRange* rangeObj = m_ViewRangeMap[actorID];//��ȡ��range����
		BodyData* bodyData = rangeObj->m_Actor;	//��ȡ����ǰ��������Ϣ
		const b2Vec2& axisPos = bodyData->GetBody()->GetPosition();
		const b2Vec2& b2Pos = bodyData->GetBody()->GetPosition();
		//����ɾ�������еľ���
		ReCalcPosition(actorID,axisPos, b2Pos);//�������
		rangeObj->SetAxisPos(b2Pos);//�������鷽��

	}
	//��ȡ����ǰ�����ڵ����г�Ա
	void GetRangeDistanceObjs(unordered_set<ActorID>& actors,const b2Vec2& range,const b2Vec2& pos)
	{
		float minX = pos.x - range.x, maxX = pos.x + range.x;//��ǰ����С��Χ
		float minY = pos.y - range.y, maxY = pos.y + range.y;//��ǰ�����Χ
		unordered_set<ActorID> compareXSet;  
		ptrdiff_t distanceX = std::distance(m_XAxisBodyMap.find(pos.x), m_XAxisBodyMap.end());
		ptrdiff_t distanceY = std::distance(m_YAxisBodyMap.find(pos.x), m_YAxisBodyMap.end());
		//���Ȼ�ȡ�����������
		for (auto item = m_XAxisBodyMap.rbegin() + (m_XAxisBodyMap.size() - distanceX); item == m_XAxisBodyMap.rend(); item++)
		{
			if (item->first > maxX)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				compareXSet.insert(*actorSet);
		}
		//���Ȼ�ȡ�����������
		for (auto item = m_XAxisBodyMap.find(pos.x); item == m_XAxisBodyMap.end(); item++)
		{
			if (item->first < minX)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				compareXSet.insert(*actorSet);
		} 
		//���Ȼ�ȡ�����������
		for (auto item = m_YAxisBodyMap.rbegin() + (m_YAxisBodyMap.size() - distanceY); item == m_YAxisBodyMap.rend(); item++)
		{
			if (item->first > maxY)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				if (compareXSet.count(*actorSet))
					actors.insert(*actorSet); 
		}
		//���Ȼ�ȡ�����������
		for (auto item = m_YAxisBodyMap.find(pos.x); item == m_YAxisBodyMap.end(); item++)
		{
			if (item->first < minY)
				break;
			for (auto actorSet = item->second.begin(); actorSet != item->second.end(); actorSet++)
				if (compareXSet.count(*actorSet))
					actors.insert(*actorSet);
		}
	}
	void UnregisterBody(BodyData* body);
	unordered_map<ActorID, BodyData*>& GetViewBody(ActorID id, ViewType type, ViewStatus view);
	//��ȡ����Ұ��Χ�� �����һ��BodyData 
	b2Body* MinumumDistanceBody(ActorID id, ViewType checkType);
	//֡��ʱ
	void DistanceCalcAdd(b2Body* body);
	void DistanceCalcAdd(BodyData* BodyData);
	void DistanceCalcRemove(b2Body* body);
	void DistanceCalcRemove(BodyData* BodyData);
	void DistanceCalcClear();
	void DistanceCalc(); 


	unordered_map<ViewType, unordered_map<ActorID, unordered_map<ViewStatus, unordered_map<ActorID,BodyData*>>>> DistanceMap;  
	//��ӡ��ǰ�ɼ�����
	void __Dump(ActorID id, ViewType type);
private:
	//���¼���
	inline void RecountDistance(b2Body* body);
	inline void RecountDistance(BodyData* body);
	//���¼������
	void __RecountDistance(BodyData* body, ViewType viewType, ViewType addType);
	//���һ����Ԫ �����е� listType �µĵ�Ԫ���뵽 viewType �£�����BodyData��Ԫ���뵽 addType��
	void __RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType);
	void __UnregisterBody(BodyData* body, ViewType viewType, ViewType addType); 
}; 
