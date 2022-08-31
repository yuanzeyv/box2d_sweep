#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <list>     
#include <math.h>
#include <stack>
//��ǰ��ɫ���Կ���˭
//��ǰ˭���Կ�����ɫ
using std::unordered_map;
using std::map;
using std::unordered_set;
using std::list;
using std::vector;
using std::stack;
class AxisDistanceManager; 
enum ViewAction
{
	NO_ATION = 0,
	PORT_ACTION = 1,
	ALL_ACTION =2
}; 
//����һ����־λ,���8λ�������������
enum PointPosType {
	POS_BODY_LIMIT_MAX = 0,//��λ���
	POS_BODY_LIMIT_MIN = 1,//��λ��С 
};
enum PointType
{
	Body_Type = 0,
	View_Type = 1,
	MAX_POINT_TYPE = 2,
};
class ViewRange {
public: 
	BodyData*  m_Actor;//��ǰ�Ľ�ɫ 
	unordered_set<ActorID> m_ObserverMap;//���Կ����Ľ�ɫ�б�
	unordered_set<ActorID> m_BeObserverMap;//��˭�۲쵽 
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  

	b2Vec2 m_BodyPos;//��¼��ɫ��һ�����ʱ��AABB   
	b2AABB m_BodyAABB;//��¼��ɫ��һ�����ʱ��AABB   

	float m_ViewMinusPercent;//�ɻ����Ӳ�,���۵�һ������ʱ,������һ�ξ������,�ұ��ξ������Ϊ�ֲ�������� 
public:
	inline ViewRange(BodyData* bodyData);
	//��ȡ����ǰ��ɫ�ĸ���AABB
	inline const b2AABB& GetNowBodyAABB()
	{
		return m_Actor->GetAABB();
	}
	inline const b2AABB& GetFrontBodyAABB()
	{
		return m_BodyAABB;
	}
	//���¼����ӿ�
	inline void RecalcViewRange();
	inline const b2Vec2& GetViewRange();
	//1����ǰ������Ӳ�
	//2����ǰ�����Ӳ�
	//3����,�Ӳ�ƫ�ƹ���,ֱ�����¼����ɫ�ӿڶ���
	inline int JudgeActorViewAction();
	inline void Reset();
	//һ���˽���Ұ
	bool EnterView(vector<ActorID> actorList);
	void LeaveView(ActorID actorID);

	//һ���˽���Ұ
	bool EnterOtherView(ActorID actorID);
	void LeaveOtherView(ActorID actorID);
	void SetAxisPos(const b2Vec2& pos);
	const b2Vec2& GetAxisPos();
	~ViewRange();
};
typedef unordered_set<ActorID> ViewRangeTypeSet[2];
typedef map<float, ViewRangeTypeSet*> ViewRangeRecordMap;
class AxisDistanceManager
{ 
public: 
	//��ǰ���еľ������
	ViewRangeRecordMap m_XAxisBodyMap;//x����Ľ�ɫ��¼ (��¼������� ��)
	ViewRangeRecordMap m_YAxisBodyMap;//x����Ľ�ɫ��¼ (��¼������� ��)  
	list<ViewRangeTypeSet*> m_IdleViewRnageMap;//���е�

	unordered_map<ActorID, ViewRange*> m_ViewRangeMap;//��ɫ��Ӧ�ķ�Χ��Ϣ
	 
	unordered_set<ActorID> m_DelayCalcMoveMap;//��ɫ��Ӧ�ķ�Χ��Ϣ
	AxisDistanceManager();
	void InitIdelRangeMap(int size);//����Ĭ�������Ŀ����б�

	inline bool RegisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id))
			return false; //�Ѿ�ע�����
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//ͨ��������Ƶ�Ԫ��ȡ����Ӧ������
		ViewRange* viewRangeObj = new ViewRange(bodyData);
		m_ViewRangeMap[id] = viewRangeObj;//ע����뼴��,���������һ֡�Ŀ�ʼ���ƶ���ɫ
		m_DelayCalcMoveMap.insert(id);//�����ɫ������һ֡���ؼ���
		return true;
	}  
	//������������,���ƶ���ɫ,�ڼ������
	void MoveActor()
	{
		for (auto item = m_DelayCalcMoveMap.begin();item != m_DelayCalcMoveMap.end();item++) {
			//���ȼ��㵱ǰ�Ĳ���
		}

		ActorID* actoArr = actoList.data();
		for (int i = 0; i < actoList.size(); i++)
		{
			ActorID actorID = actoArr[i];
			if (!m_ViewRangeMap.count(actorID)) {
				printf("������һ������\n\r");
				continue;
			}
			ViewRange* actoViewange = m_ViewRangeMap[actorID];
			//��ȡ�������ӿ�֮ǰ���ӿ�
			const b2AABB frontViewRange = actoViewange->GetViewRange();//֮ǰ���ӿ�
			//ɾ����ǰ��λ��
			RemoveDistancePoint(actorID, frontViewRange);
			//���¼��㵱ǰ��Ԫ���ӿ�,���Ҹ�����
			actoViewange->RecalcViewRange();//���¼����ӿ�
			const b2AABB& viewRange = actoViewange->GetViewRange();//��ȡ����ǰ���ӿ�
			AdditionDistancePoint(actorID, viewRange);
		}
	}
	inline void RemoveDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
	{
		if (m_XAxisBodyMap.count(viewRange.lowerBound.x))//���ڵ�ǰ�����Ļ� 
			if (m_XAxisBodyMap[viewRange.lowerBound.x][addType]->count(actorID))
				m_XAxisBodyMap[viewRange.lowerBound.x][addType]->erase(actorID);
		if (m_XAxisBodyMap.count(viewRange.upperBound.x))//���ڵ�ǰ�����Ļ� 
			if (m_XAxisBodyMap[viewRange.upperBound.x][addType]->count(actorID))
				m_XAxisBodyMap[viewRange.upperBound.x][addType]->erase(actorID); 
		if (m_YAxisBodyMap.count(viewRange.lowerBound.y))//���ڵ�ǰ�����Ļ� 
			if (m_YAxisBodyMap[viewRange.lowerBound.y][addType]->count(actorID))
				m_YAxisBodyMap[viewRange.lowerBound.y][addType]->erase(actorID);
		if (m_YAxisBodyMap.count(viewRange.upperBound.y))//���ڵ�ǰ�����Ļ� 
			if (m_YAxisBodyMap[viewRange.upperBound.y][addType]->count(actorID))
				m_YAxisBodyMap[viewRange.upperBound.y][addType]->erase(actorID);
		//�ж��Ƿ�ʹ�ý�����
		ViewRnageUserOver(viewRange.lowerBound.x, true, true);
		ViewRnageUserOver(viewRange.upperBound.x, true, true);
		ViewRnageUserOver(viewRange.lowerBound.y, false, true);
		ViewRnageUserOver(viewRange.upperBound.y, false, true); 
	}
	inline void ViewRnageUserOver(float pos,bool isX,bool virify = true)
	{
		ViewRangeRecordMap& tempMap = isX?m_YAxisBodyMap: m_YAxisBodyMap; 
		if (virify && tempMap.count(pos) == 0)
			return;
		ViewRangeTypeSet* rangeTypeSet = tempMap[pos];
		bool isEmpty = true;
		for (int i = 0;i < MAX_POINT_TYPE;i++) {
			if (!rangeTypeSet[i]->empty()) {
				isEmpty = false;
				break;
			}
		}
		if (!isEmpty)
			return;
		m_IdleViewRnageMap.push_back(rangeTypeSet);
		tempMap.erase(pos); 
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
		if (!m_ViewRangeMap.count(actorID))//��ɫû���Ļ�
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
	inline void AdditionDistancePoint(ActorID actorID, const b2AABB& viewRange)
	{
		if (viewRange.lowerBound.x == viewRange.upperBound.x)
			m_XAxisBodyMap[viewRange.lowerBound.x][actorID] = DirectionType::DIR_SHARE;
		else
		{
			m_XAxisBodyMap[viewRange.lowerBound.x][actorID] = DirectionType::DIR_LEFT;
			m_XAxisBodyMap[viewRange.upperBound.x][actorID] = DirectionType::DIR_RIGHT;
		}

		if (viewRange.lowerBound.y == viewRange.upperBound.y)
			m_YAxisBodyMap[viewRange.upperBound.y][actorID] = DirectionType::DIR_SHARE;
		else
		{
			m_YAxisBodyMap[viewRange.lowerBound.y][actorID] = DirectionType::DIR_DOWN;
			m_YAxisBodyMap[viewRange.upperBound.y][actorID] = DirectionType::DIR_TOP;
		}
	}

	//�������
	void CalcDistance(ActorID actorID)
	{
		if (!m_ViewRangeMap.count(actorID))
			return;
		ViewRange* actoViewange = m_ViewRangeMap[actorID];
		int action = actoViewange->JudgeActorViewAction();//�жϵ�ǰӦ��ʹ�����ֶ���
		switch (action)
		{
		case ViewAction::NO_ATION:
			break;
		case ViewAction::PORT_ACTION:
		{
			//
			//����ע�ᵱǰ��ǰ��ɫ�ľ���
			//������ƫ�ƾ���
			//��ѯƫ�ƾ����ڵ�
		}
		break;
		case ViewAction::ALL_ACTION:
			break; 
		}

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
}; 
