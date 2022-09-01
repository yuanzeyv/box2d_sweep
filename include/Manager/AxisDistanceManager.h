#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <list>     
#include <math.h> 
#include <typeinfo>
//��ǰ��ɫ���Կ���˭
//��ǰ˭���Կ�����ɫ
using std::unordered_map;
using std::map;
using std::unordered_set;
using std::list;
using std::vector;
#define MAX_OVERFLOW_RANGE (1.0f) //����1�׵ȷ� 
/*
bit 56-64 ʣ������
bit 48-55 ����ID
bit 0-54  ��ɫID
*/
#define AREA_ID(actorID) (((actorID) & (0xff << 48)) >> 48) //ȡ�õ�ǰ������ID
#define AABB_POINT_TYPE(actorID) ((actorID) & (0x1 << 48) > 0) //��ǰ�ĵ�λ����

#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | (type << 48)) //����һ�����õ�λ���͵�����
#define GEN_AABB_AREA_ID(actorID,id) ((actorID) | (type << 48)) //����һ�����õ�λ���͵�����
 
 
//�ж�����AABB�Ƿ������������
//����һ�� X Y���ڷ�Χ��
//x�ڷ�Χ��,y����,������С �� ����������ǰ��AABB
//x���ڷ�Χ��,���������ΧѰ�����ĵ�λ,��Ѱ���п����ҵ���,ѭ��������������λ����С��λ�Ƿ�С��
//����һ����־λ,���8λ�������������
enum PointPosType {
	POS_BODY_LIMIT_MAX = 0,//��λ���
	POS_BODY_LIMIT_MIN = 1,//��λ��С 
};
enum PointType{
	Body_Type = 0,
	View_Type = 1,
	MAX_POINT_TYPE = 2,
};

typedef unordered_set<ActorID> ViewRangeTypeSet[2];
typedef map<float, ViewRangeTypeSet*> ViewRangeRecordMap;

template<typename Type>
class AxisControl {
public;
	 map<float, Type> m_AxisRangeMap;//Ĭ�������ɴ�С
	 //��ѯĳһ��Χ��������Ϣ
	 void GetRangeAllData(list<Type>&outData, float axisKey,float findWdith,float leftOffset,float rightOffset)
	 {

		 //�����ҵ���ǰƫ�����������
		 m_AxisRangeMap
	 }
};

template<typename ListType>
class AutomaticGenerator{
public: 
	AutomaticGenerator():m_AllocStep(100) {
	}
	~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size())
			printf("��ǰ��%s����%lld��δ������,������ڴ�й©,��������߼�\n\r", typeid(ListType).name);
		for (auto item = m_IdleList.begin();item != m_IdleList.end();item++)
			delete* item;//ɾ���� 
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //ȫ������
	}
	ListType* RequireObj(){
		if (m_IdleList.size() == 0)//�б���˵Ļ�
			GenerateIdelRangeMap(m_AllocStep);//1000����ɫ  
		auto beginObj = m_IdleList.begin();
		ListType* listTypeObj = *beginObj;
		m_IdleList.erase(beginObj);
		return *listTypeObj;
	}
	void BackObj(ListType* obj) {
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep;i++) {
			ListType* typeObj = new ListType;
			m_IdleList.insert(typeObj);//������ô������ж���
		}
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//���䲽��
	int64 m_AllocCount;//����ĸ���
	std::list<ListType*> m_IdleList;//��ǰ���еĸ���
};

class ViewRange {
public: 
	BodyData*  m_Actor;//��ǰ�Ľ�ɫ 
	unordered_set<ActorID> m_ObserverMap;//���Կ����Ľ�ɫ�б� 
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  
	 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��
	b2AABB m_BodyAABB;//��¼��ɫ��һ�����ʱ��AABB    
	b2AABB m_ViewAABB;//��¼��һ�����ʱ����ͼAABB

public:
	ViewRange(BodyData* bodyData) {
		Reset();//����һ������
		m_Actor = bodyData;
	}
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_BodyPos.Set(MAX_DISTANCE, MAX_DISTANCE);
		m_ObserverMap.clear(); 
	} 

	//��ȡ����ǰ��ɫ�ĸ���AABB 
	inline const b2AABB& GetBodyAABB()
	{
		return m_BodyAABB;
	}
	inline const b2AABB& GetViewAABB()
	{
		return m_ViewAABB;
	}
	//���¼����ӿ�
	inline void RecalcViewRange()
	{
		m_BodyPos = m_Actor->GetBody()->GetPosition();
		m_ViewAABB.lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);
		m_ViewAABB.upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
	}
	//���¼����ӿ�
	inline void RecalcBodyAABB()
	{
		m_BodyAABB = m_Actor->GetAABB();
	}

	inline void RecalcBodyPosition()
	{
		m_BodyPos = m_Actor->GetBody()->GetPosition();
	}
	//����ǰ�۲쵽�����н�ɫ
	inline void ClearObserverTable() {
		m_ObserverMap.clear();
	} 
	inline const b2Vec2& GetViewRange() 
	{
		return m_ObserverRange;
	}
	//1����ǰ������Ӳ�
	//2����ǰ�����Ӳ� 
	inline int JudgeActorViewAction()
	{
		b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
		float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
		float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
		float xPercent = xMinus / (m_ObserverRange.x * 2);
		float yPercent = yMinus / (m_ObserverRange.y * 2);
		float finalPercent = std::max(xPercent, yPercent);
		if (finalPercent <= 3) {//�Ӳ��������ƶ�����С�ڰٷ�֮3,�������� 
			return 0;
		}
		return 1;
	}
	//һ���˽���Ұ
	bool EnterView(ActorID actorID)
	{
		m_ObserverMap.insert(actorID);
	}
	void LeaveView(ActorID actorID) 
	{
		m_ObserverMap.erase(actorID);
	}
	void SetBodyPos(const b2Vec2& pos)
	{ 
		m_BodyPos.Set(pos.x, pos.y);
	}
	const b2Vec2& GetBodyPos()
	{
		return m_BodyPos;
	}
	~ViewRange()
	{ 
	}
};

class AxisDistanceManager
{
public:
	//��ǰ���еľ������
	ViewRangeRecordMap m_XAxisBodyMap;//x����Ľ�ɫ��¼ (��¼������� ��)
	ViewRangeRecordMap m_YAxisBodyMap;//x����Ľ�ɫ��¼ (��¼������� ��)  

	unordered_map<ActorID, ViewRange*> m_ViewRangeMap;//��ɫ��Ӧ�ķ�Χ��Ϣ

	  
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //�Զ���B2AABB�������
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//�Զ���������ͼ�������Ķ���

	unordered_set<ActorID> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
	AxisDistanceManager() {
	} 

	inline void AddtionCalcBody(ActorID id){
		m_DelayCalcMoveList.insert(id);//�����ɫ������һ֡���ؼ���
	}

	inline bool RegisterBody(ActorID id){
		if (m_ViewRangeMap.count(id))//�����Ѿ�ע��
			return false;
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//ͨ��������Ƶ�Ԫ��ȡ����Ӧ������
		ViewRange* viewRangeObj = new ViewRange(bodyData);
		m_ViewRangeMap[id] = viewRangeObj;//ע����뼴��,���������һ֡�Ŀ�ʼ���ƶ���ɫ
		AddtionCalcBody(id);
		return true;
	}
	void RecalcActorDistance()
	{
		MoveActor();//�ƶ���ɫ
		CalcViewObj();//�����ɫ���Թ۲쵽����ͼ���� 
	}
	//������������,�������ƶ����ɫ��,�ټ������
	void MoveActor()
	{
		ViewRange* actoViewange = NULL;
		for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) {
			//���ȼ��㵱ǰ�Ĳ��� 
			ActorID actorID = *item;
			if (!m_ViewRangeMap.count(actorID)) {
				goto erase;
			}
			actoViewange = m_ViewRangeMap[actorID];
			if (actoViewange->JudgeActorViewAction() == 1) {//ɾ������Ҫ����ĵ�Ԫ
				goto erase;
			} 
			//�Ե�ǰ��ɫ����λ��,�ƶ���Ұ  �ƶ���ײ��
			RemoveDistancePoint(actorID, PointType::Body_Type, actoViewange->GetBodyAABB());//ɾ��ԭ�е�������ײ����
			RemoveDistancePoint(actorID, PointType::View_Type, actoViewange->GetViewAABB());//ɾ��ԭ�е�ʯͷ��ײ����
			actoViewange->RecalcViewRange();//�ؼ���AABB
			actoViewange->RecalcBodyAABB();//�ؼ�����ͼAABB
			actoViewange->RecalcBodyPosition();//�ؼ����ɫ��λ��
			AdditionDistancePoint(actorID, PointType::Body_Type, actoViewange->GetBodyAABB());//�����е������ȥ
			AdditionDistancePoint(actorID, PointType::View_Type, actoViewange->GetViewAABB());//�����е������ȥ 
			item = item++; 
		erase:
			m_DelayCalcMoveList.erase(actorID);
		} 
	}
	//�ؼ���
	inline void CalcViewObj()
	{	//������һ��ɸѡ��,�������ı��ƶ��ĵ�Ԫset
		for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) { 
			ActorID actorID = *item;//���ȼ��㵱ǰ�Ĳ��� 
			ViewRange* actoViewange = m_ViewRangeMap[actorID];//��ȡ����ɫ��ʹͽ��Ϣ
			actoViewange->ClearObserverTable();//����ǰ�Ŀɼ�����
			const b2AABB& bodyViewAABB = actoViewange->GetViewAABB();//��ȡ����ɫ����ͼ��Χ
			unordered_set<ActorID> visibleSet;
			auto xBegin = m_XAxisBodyMap[bodyViewAABB.lowerBound.x];//�ӵ�һֱ�ҵ���
			//Ѱ����ͼ��Χ�ڵ����нڵ�
			for (auto xBegin = m_XAxisBodyMap.find(bodyViewAABB.lowerBound.x);xBegin != m_XAxisBodyMap.end();xBegin++) {
				if (xBegin->first > bodyViewAABB.upperBound.x)
					break;
				unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.x][PointType::Body_Type];
				if (actorList->size() == 0)
					continue; 
				for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
					visibleSet.insert(*objItem);
				} 
			}
			//Ѱ��y��
			for (auto yBegin = m_YAxisBodyMap.find(bodyViewAABB.lowerBound.y);yBegin != m_YAxisBodyMap.end();yBegin++) {
				if (yBegin->first > bodyViewAABB.upperBound.y)
					break;
				unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.y][PointType::Body_Type];
				if (actorList->size() == 0) 
					continue; 
				for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
					if (visibleSet.count(*objItem) == 0)
						continue;
					visibleSet.erase(*objItem);
					actoViewange->EnterView(*objItem);
				}
			} 
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
	//��ʹ�ñ�����֮ǰӦȷ��,��ǰ���б���,���޶�Ӧ��ɫ
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
	{ 
		//�ж�����,����
		if (!m_XAxisBodyMap.count(viewRange.lowerBound.x)){//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
			m_XAxisBodyMap[viewRange.lowerBound.x] = typeSet;
		}
		if (!m_XAxisBodyMap.count(viewRange.upperBound.x)){//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
			m_XAxisBodyMap[viewRange.upperBound.x] = typeSet;
		}
		if (!m_YAxisBodyMap.count(viewRange.lowerBound.y)) {//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
			m_YAxisBodyMap[viewRange.lowerBound.y] = typeSet;
		}
		if (!m_YAxisBodyMap.count(viewRange.upperBound.y)) {//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
			m_YAxisBodyMap[viewRange.upperBound.y] = typeSet;
		}
		m_XAxisBodyMap[viewRange.lowerBound.x][addType]->insert(actorID);
		m_XAxisBodyMap[viewRange.upperBound.x][addType]->insert(actorID);
		m_YAxisBodyMap[viewRange.lowerBound.y][addType]->insert(actorID);
		m_YAxisBodyMap[viewRange.upperBound.y][addType]->insert(actorID); 
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
		m_IdleViewRangeMap.push_back(rangeTypeSet);
		tempMap.erase(pos); 
	}
	void UnregisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id))
			return ; //�Ѿ�ע����� 
		ViewRange* rangeObj = m_ViewRangeMap[id];
		RemoveDistancePoint(id, PointType::Body_Type, rangeObj->GetBodyAABB());
		RemoveDistancePoint(id, PointType::View_Type, rangeObj->GetViewAABB());
		delete m_ViewRangeMap[id];
		m_ViewRangeMap.erase(id);
	} 
	 
}; 
