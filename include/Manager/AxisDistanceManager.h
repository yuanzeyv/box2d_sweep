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
#define MAX_OVERFLOW_RANGE (0.5f) //����1�׵ȷ� 
/*
bit 56-64 ʣ������
bit 48-55 ����ID
bit 0-54  ��ɫID
*/
#define AREA_ID(actorID) (((actorID) & (0xff << 48)) >> 48) //ȡ�õ�ǰ������ID
#define AABB_POINT_TYPE(actorID) ((actorID) & (0x1 << 48) > 0) //��ǰ�ĵ�λ����

#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ( (type & 1) << 56)) //����һ�����õ�λ���͵����� 
#define GEN_AABB_ID(actorID,index) (actorID | ((index & 0xffff) << 48 ))//����һ��ID
enum PointPosType {
	POS_BODY_LIMIT_MAX = 0,//��λ���
	POS_BODY_LIMIT_MIN = 1,//��λ��С 
};
enum AxisType {
	X_AXIS = 0,//x��
	Y_AXIS = 1,//y��
	MAX_AXIS = 2//��������Ϣ

};
enum PointType {
	BODY_TYPE = 0,
	VIEW_TYPE = 1,
	MAX_POINT_TYPE = 2,
}; 
class ViewRangeTypeSet {
public:
	unordered_set<ActorID> m_TypeSet[PointType::MAX_POINT_TYPE];
	ViewRangeTypeSet() {}
	unordered_set<ActorID>& operator[](PointType type) {
		return m_TypeSet[type];
	}
	unordered_set<ActorID>& GetRangeType(PointType type) {
		return m_TypeSet[type];
	}
}; 
typedef map<float, ViewRangeTypeSet*> ViewRangeRecordMap;
   
template<typename ListType>
class AutomaticGenerator {
public:
	AutomaticGenerator() :m_AllocStep(100) {
	}
	~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size()) {
			printf("��ǰ��%s����%lld��δ������,������ڴ�й©,��������߼�\n\r", typeid(ListType).name(), m_AllocCount - m_IdleList.size());
		}
		for (auto item = m_IdleList.begin(); item != m_IdleList.end(); item++)
			delete* item;//ɾ���� 
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //ȫ������
	}
	ListType* RequireObj() {
		if (m_IdleList.size() == 0)//�б���˵Ļ�
			GenerateIdelListTypes();//1000����ɫ  
		auto beginObj = m_IdleList.begin();
		ListType* listTypeObj = *beginObj;
		m_IdleList.erase(beginObj);
		return listTypeObj;
	}
	void BackObj(ListType* obj) {
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++) {
			ListType* typeObj = new ListType();
			m_IdleList.push_back(typeObj);//������ô������ж���
		}
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//���䲽��
	int64_t m_AllocCount;//����ĸ���
	std::list<ListType*> m_IdleList;//��ǰ���еĸ���
};
class ViewRange {
public: 
	BodyData*  m_Actor;//��ǰ�Ľ�ɫ 
	unordered_set<ActorID> m_ObserverMap;//���Կ����Ľ�ɫ�б� 
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  
	 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��
	b2AABB m_BodyAABB[PointType::MAX_POINT_TYPE];//��¼��ɫ��һ�����ʱ��AABB     
	list<b2AABB*> m_SplitAABBList[PointType::MAX_POINT_TYPE];//���յ�,�ָ���AABB

	AutomaticGenerator<b2AABB>* m_AABBAllocObj;//����һ�·���AABB�Ķ���
public:
	ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj) {
		Reset();//����һ������
		m_Actor = bodyData;
		m_AABBAllocObj = allocObj;  
	}
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_BodyPos.Set(MAX_DISTANCE, MAX_DISTANCE);
		m_ObserverMap.clear(); 
	} 

	//��ȡ����ǰ��ɫ�ĸ���AABB 
	inline const b2AABB& GetBodyAABB(PointType type)
	{
		return m_BodyAABB[type];
	} 
	//��ȡ����ǰ����ͼ�б�
	const list<b2AABB*>& GetSplitAABB(PointType type)
	{ 
		return m_SplitAABBList[type];
	}
	//���¼����ӿ�
	inline void RecalcAABBRange(PointType type)
	{
		if(type == PointType::VIEW_TYPE){
			m_BodyPos = m_Actor->GetBody()->GetPosition();
			m_BodyAABB[type].lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);
			m_BodyAABB[type].upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
		} else {
			const b2AABB& bodyAABB = m_Actor->GetAABB();
			m_BodyAABB[type].lowerBound.Set(bodyAABB.lowerBound);
			m_BodyAABB[type].upperBound.Set(bodyAABB.upperBound); 
		}
	}
	inline void RecalcBodyPosition()
	{
		m_BodyPos = m_Actor->GetBody()->GetPosition();
	} 
	//����ָ����(AABB�ᾭ����,��ͼ��������)
	inline void CalcSplitAABB(PointType type)//����ָ�AABB
	{   //���Ȼ�ȡ����ǰ�����AABB�ĳ���
		float length = m_BodyAABB[type].upperBound.x - m_BodyAABB[type].lowerBound.x;
		int count = ceil(length / MAX_OVERFLOW_RANGE);//������Ҫ������ٸ�  108
		int splitCount = count - m_SplitAABBList[type].size();//��ȡ����ǰ�ķָ����
		if (splitCount < 0) {
			for (int i = 0; i > splitCount; i--) { 
				auto obj = m_SplitAABBList[type].begin();
				m_AABBAllocObj->BackObj(*obj);
				m_SplitAABBList[type].erase(obj);
			} 
		} else if (splitCount > 0) {
			for (int i = 0; i > splitCount; i--) {
				auto obj = m_SplitAABBList[type].begin();
				m_SplitAABBList[type].push_back(m_AABBAllocObj->RequireObj());
			}
		}
		float calcX = m_BodyAABB[type].lowerBound.x;//�ʼ��λ��
		for (auto item = m_SplitAABBList[type].begin(); item != m_SplitAABBList[type].end(); item++)
		{
			b2AABB* aabb = *item;
			aabb->lowerBound.Set(calcX, m_BodyAABB[type].lowerBound.y);
			if (calcX + MAX_OVERFLOW_RANGE < m_BodyAABB[type].upperBound.x) 
				aabb->upperBound.Set(calcX + MAX_OVERFLOW_RANGE,m_BodyAABB[type].upperBound.y);
			else
				aabb->upperBound.Set(m_BodyAABB[type].upperBound.y, m_BodyAABB[type].upperBound.y); 
		}
	} 
	//����ָ���ͼ����
	//���¼����ӿ�
	inline void RecalcBodyAABB(PointType type)
	{
		RecalcAABBRange(type);//���������AABB
		CalcSplitAABB(type);//�����и���AABB
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
	const b2Vec2& GetBodyPos()
	{
		return m_BodyPos;
	}
	//���յ�λ
	void RecycleAABBPoint(PointType type)
	{
		auto item = m_SplitAABBList->begin();
		while (item != m_SplitAABBList->end()) {
			m_AABBAllocObj->BackObj(*item);
			item = m_SplitAABBList->erase(item);
		}
	}
	~ViewRange()
	{
		RecycleAABBPoint(PointType::BODY_TYPE);
		RecycleAABBPoint(PointType::VIEW_TYPE);
	}
};
//�ù۲���ģʽ,��ʵ�ֵ�ĳһ��ɫ�ƶ���,�۲쵽�����,��Ҫ��ĳЩ����
//��һ���н���ģʽ,����֪ͨ���еĹ۲���
class 

class AxisDistanceManager
{
public:  
	//�ڴ�������
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //�Զ���B2AABB�������
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//�Զ���������ͼ�������Ķ��� 
	//��ǰ���еľ������
	ViewRangeRecordMap m_AxisBodyMap[AxisType::MAX_AXIS];//������X�����к�Y������ 
	unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ

	unordered_set<ActorID> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
	AxisDistanceManager(){}
	inline void AddtionCalcBody(ActorID id){
		m_DelayCalcMoveList.insert(id);//�����ɫ������һ֡���ؼ���
	} 
	inline bool RegisterBody(ActorID id) {
		if (m_ViewObjMap.count(id)) return false;//�����Ѿ�ע�� 
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//ͨ��������Ƶ�Ԫ��ȡ����Ӧ������ 
		m_ViewObjMap[id] = new ViewRange(bodyData, &m_AABBIdleGenerate);//ע����뼴��
		AddtionCalcBody(id);//����һ֡���¼����ɫ��Ϣ
		return true;
	}
	//���¼����ɫ����
	void RecalcActorDistance(){
		//�������һ�µ�ǰ���ƶ�����
		MoveActor();//�������н�ɫ���Ӿ�,������ײAABB 
		CalcViewObj();//�����ɫ���Թ۲쵽����ͼ���� 
	}
	inline void RemoveDistanceAABB(ViewRange* actoViewRange, PointType type) {
		const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//��ȡ����ǰ���к��AABB
		int index = 1;
		ActorID actorID = actoViewRange->m_Actor->ID();
		for (auto item = splitAABBList.begin();item != splitAABBList.end();item++, index++) {
			ActorID reActorID = GEN_AABB_ID(actorID, index);//��ȡ����ǰ������ID
			RemoveDistancePoint(actorID, type, *(*item));//ɾ��ԭ�е�������ײ����
		}
	}
	inline void AdditionDistanceAABB(ViewRange* actoViewRange, PointType type) {
		const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//��ȡ����ǰ���к��AABB
		int index = 1;
		ActorID actorID = actoViewRange->m_Actor->ID();
		for (auto item = splitAABBList.begin();item != splitAABBList.end();item++, index++) {
			ActorID reActorID = GEN_AABB_ID(actorID, index);//��ȡ����ǰ������ID
			AdditionDistancePoint(actorID, type, *(*item));//ɾ��ԭ�е�������ײ����
		}
	}
	//��ʹ�ñ�����֮ǰӦȷ��,��ǰ���б���,���޶�Ӧ��ɫ
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
	{
		ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //������СID
		ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //�������ID 
		ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
		ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];
		//�ж�����,����
		if (!xRangeRecordMap.count(viewRange.lowerBound.x)) {//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
			xRangeRecordMap[viewRange.lowerBound.x] = typeSet;
		}
		if (!xRangeRecordMap.count(viewRange.upperBound.x)) {//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
			xRangeRecordMap[viewRange.upperBound.x] = typeSet;
		}
		if (!yRangeRecordMap.count(viewRange.lowerBound.y)) {//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
			yRangeRecordMap[viewRange.lowerBound.y] = typeSet;
		}
		if (!yRangeRecordMap.count(viewRange.upperBound.y)) {//�����ڵ�ǰ�����Ļ�  
			ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
			yRangeRecordMap[viewRange.upperBound.y] = typeSet;
		}
		xRangeRecordMap[viewRange.lowerBound.x]->GetRangeType(addType).insert(actorID);
		xRangeRecordMap[viewRange.upperBound.x]->GetRangeType(addType).insert(actorID);
		yRangeRecordMap[viewRange.lowerBound.y]->GetRangeType(addType).insert(actorID);
		yRangeRecordMap[viewRange.upperBound.y]->GetRangeType(addType).insert(actorID);
	}
	inline void RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange)
	{
		ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //������СID
		ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //�������ID 
		ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
		ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];
		if (xRangeRecordMap.count(viewRange.lowerBound.x))//ɾ�����ID��Ӧ��X�����
			if (xRangeRecordMap[viewRange.lowerBound.x]->GetRangeType(type).count(minActorID))
				xRangeRecordMap[viewRange.lowerBound.x]->GetRangeType(type).erase(minActorID);
		if (xRangeRecordMap.count(viewRange.upperBound.x))//���ڵ�ǰ�����Ļ� 
			if (xRangeRecordMap[viewRange.upperBound.x]->GetRangeType(type).count(maxActorID))
				xRangeRecordMap[viewRange.upperBound.x]->GetRangeType(type).erase(maxActorID);
		if (yRangeRecordMap.count(viewRange.lowerBound.y))//���ڵ�ǰ�����Ļ� 
			if (yRangeRecordMap[viewRange.lowerBound.y]->GetRangeType(type).count(minActorID))
				yRangeRecordMap[viewRange.lowerBound.y]->GetRangeType(type).erase(minActorID);
		if (yRangeRecordMap.count(viewRange.upperBound.y))//���ڵ�ǰ�����Ļ� 
			if (yRangeRecordMap[viewRange.upperBound.y]->GetRangeType(type).count(maxActorID))
				yRangeRecordMap[viewRange.upperBound.y]->GetRangeType(type).erase(maxActorID);
		//�ж��Ƿ�ʹ�ý�����
		ABBBRnageRecycle(viewRange.lowerBound.x, true);
		ABBBRnageRecycle(viewRange.upperBound.x, true);
		ABBBRnageRecycle(viewRange.lowerBound.y, false);
		ABBBRnageRecycle(viewRange.upperBound.y, false);
	}
	inline void ABBBRnageRecycle(float pos, bool isX)
	{
		ViewRangeRecordMap& tempMap = isX ? m_AxisBodyMap[AxisType::X_AXIS] : m_AxisBodyMap[AxisType::Y_AXIS];
		if (!tempMap.count(pos))
			return;
		ViewRangeTypeSet* rangeTypeSet = tempMap[pos];
		bool isEmpty = true;
		for (int type = 0;type < MAX_POINT_TYPE;type++) {
			if (!rangeTypeSet->GetRangeType((PointType)type).empty()) {
				isEmpty = false;
				break;
			}
		}
		if (!isEmpty)
			return;
		m_ViewCellIdleGenerate.BackObj(rangeTypeSet);//������
		tempMap.erase(pos);
	}
	void MoveActor()
	{
		//�ƶ���ɫ,�����ɫ�����ƶ�������,�������һ���Ӿ෶Χ���ж�
		ViewRange* actoViewange = NULL;
		bool OnlyCalculate = false;
		for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) {
			//���ȼ��㵱ǰ�Ĳ��� 
			ActorID actorID = *item;
			OnlyCalculate = true;
			if (!m_ViewObjMap.count(actorID))//���󲻴���,ֱ��ɾ��
				goto erase; 
			actoViewange = m_ViewObjMap[actorID];
			if (actoViewange->JudgeActorViewAction() == 1){//�ƶ������ǳ
				goto calc_erase;//�����ɾ��
			} 
			OnlyCalculate = true;
		calc_erase: 
			//�Ե�ǰ��ɫ����λ��,�ƶ���Ұ  �ƶ���ײ��
			RemoveDistanceAABB(actoViewange, PointType::BODY_TYPE);//ɾ��body
			RemoveDistanceAABB(actoViewange, PointType::VIEW_TYPE);//ɾ��view 
			actoViewange->RecalcBodyAABB(PointType::BODY_TYPE);//���¼��㵱ǰ����ײ����
			actoViewange->RecalcBodyAABB(PointType::VIEW_TYPE);//���¼��㵱ǰ����ͼ���� 
			actoViewange->RecalcBodyPosition();//�ؼ����ɫ��λ��
			AdditionDistanceAABB(actoViewange, PointType::BODY_TYPE);//�����е������ȥ
			AdditionDistanceAABB(actoViewange, PointType::VIEW_TYPE);//�����е������ȥ 
			item = item++; 
			if (OnlyCalculate)
				continue;
		erase:
			m_DelayCalcMoveList.erase(actorID); 
		}
	}
	//�ؼ���
	inline void CalcViewObj()
	{	

		//�ƶ���ɫ,��Ϊ����
		//1:����ɾ����ǰ��ɫ���ܿ������������
		//1-1:���¼���,��ͼ��Χ��,�������������Ŀ

		//2:֪ͨ��ǰ�۲쵽�ý�ɫ�����,��ɫ�������ƶ�,Ȼ������޸�
		
		//������һ��ɸѡ��,�������ı��ƶ��ĵ�Ԫset
		//for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) {
		//	ActorID actorID = *item;//���ȼ��㵱ǰ�Ĳ��� 
		//	ViewRange* actoViewange = m_ViewObjMap[actorID];//��ȡ����ɫ��ʹͽ��Ϣ
		//	actoViewange->ClearObserverTable();//����ǰ�Ŀɼ�����
		//	const b2AABB& bodyViewAABB = actoViewange->GetViewAABB();//��ȡ����ɫ����ͼ��Χ
		//	unordered_set<ActorID> visibleSet;
		//	auto xBegin = m_XAxisBodyMap[bodyViewAABB.lowerBound.x];//�ӵ�һֱ�ҵ���
		//	//Ѱ����ͼ��Χ�ڵ����нڵ�
		//	for (auto xBegin = m_XAxisBodyMap.find(bodyViewAABB.lowerBound.x);xBegin != m_XAxisBodyMap.end();xBegin++) {
		//		if (xBegin->first > bodyViewAABB.upperBound.x)
		//			break;
		//		unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.x][PointType::Body_Type];
		//		if (actorList->size() == 0)
		//			continue;
		//		for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
		//			visibleSet.insert(*objItem);
		//		}
		//	}
		//	//Ѱ��y��
		//	for (auto yBegin = m_YAxisBodyMap.find(bodyViewAABB.lowerBound.y);yBegin != m_YAxisBodyMap.end();yBegin++) {
		//		if (yBegin->first > bodyViewAABB.upperBound.y)
		//			break;
		//		unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.y][PointType::Body_Type];
		//		if (actorList->size() == 0)
		//			continue;
		//		for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
		//			if (visibleSet.count(*objItem) == 0)
		//				continue;
		//			visibleSet.erase(*objItem);
		//			actoViewange->EnterView(*objItem);
		//		}
		//	}
		//}
	} 
	void UnregisterBody(ActorID id)
	{
		if (m_ViewObjMap.count(id))
			return ; //�Ѿ�ע�����  
		ViewRange* rangeObj = m_ViewObjMap[id];
		RemoveDistanceAABB(rangeObj, PointType::BODY_TYPE);
		RemoveDistanceAABB(rangeObj, PointType::VIEW_TYPE); 
		delete rangeObj;
		m_ViewObjMap.erase(id);
	}  
}; 
