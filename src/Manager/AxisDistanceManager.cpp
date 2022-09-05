#pragma once
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h"  
#include "Manager/AxisDistanceManager.h"
#include <map> 
#include <vector>   
using namespace std;   

ViewRange::ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range = b2Vec2(0.5, 0.5)) :m_ObserverRange(range), m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), m_AABBAllocObj(allocObj)
{
} 

const b2AABB& ViewRange::GetBodyAABB(PointType type)//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_BodyAABB[type];
}

const list<b2AABB*>& ViewRange::GetSplitAABB(PointType type)//��ȡ����ǰ�Ĳü�AABB
{
	return m_SplitAABBList[type];
}

void ViewRange::RecalcAABBRange(PointType type)//���¼����ӿ�
{
	if (type == PointType::VIEW_TYPE) {
		m_BodyPos = m_Actor->GetBody()->GetPosition();
		m_BodyAABB[type].lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);
		m_BodyAABB[type].upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
	}
	else {
		const b2AABB& bodyAABB = m_Actor->GetAABB();
		m_BodyAABB[type].lowerBound.Set(bodyAABB.lowerBound);
		m_BodyAABB[type].upperBound.Set(bodyAABB.upperBound);
	}
}

void ViewRange::RecalcBodyPosition()
{
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}

void ViewRange::CalcSplitAABB(PointType type)//����ָ����(AABB�ᾭ����,��ͼ��������)
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
	}
	else if (splitCount > 0) {
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
			aabb->upperBound.Set(calcX + MAX_OVERFLOW_RANGE, m_BodyAABB[type].upperBound.y);
		else
			aabb->upperBound.Set(m_BodyAABB[type].upperBound.y, m_BodyAABB[type].upperBound.y);
	}
}

void ViewRange::RecalcBodyAABB(PointType type)//���¼����ӿ�
{
	RecalcAABBRange(type);//���������AABB
	CalcSplitAABB(type);//�����и���AABB
}

void ViewRange::ClearObserverTable() //����ǰ�۲쵽�����н�ɫ
{
	m_ObserverMap.clear();
}

const b2Vec2& ViewRange::GetViewRange() //��ȡ����ǰ����ͼ��Χ
{
	return m_ObserverRange;
}

int ViewRange::JudgeActorViewAction()//�����Ӳ�,���ڶ��ٲŻ��ؼ���
{
	b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float xPercent = xMinus / (m_ObserverRange.x * 2);
	float yPercent = yMinus / (m_ObserverRange.y * 2);
	float finalPercent = std::max(xPercent, yPercent);
	if (finalPercent <= 3)//�Ӳ��������ƶ�����С�ڰٷ�֮3,�������� 
		return 0;
	return 1;
}

BodyData* ViewRange::GetActor() {//��ȡ����ǰ�Ľ�ɫ
	return m_Actor;
}

bool ViewRange::EnterView(ViewRange* actor)//ĳһ����ɫ������Ұ
{
	m_ObserverMap[actor->GetActor()->ID()] = actor;
	actor->EnterBeObseverView(this);//���˹۲쵽���Լ�
}

bool ViewRange::EnterBeObseverView(ViewRange* actor)//ĳһ����ɫ���뵽�˱��۲����ͼ
{
	m_BeObserverMap[actor->GetActor()->ID()] = actor;
}

void ViewRange::LeaveView(ActorID actorID) //ĳһ����ɫ�뿪����ͼ
{
	m_ObserverMap.erase(actorID);
}

const b2Vec2& ViewRange::GetBodyPos()//��ȡ����ǰ��ҵĵ�λ
{
	return m_BodyPos;
}

void ViewRange::RecycleAABBPoint(PointType type)//���յ�λ
{
	for (auto item = m_SplitAABBList->begin(); item != m_SplitAABBList->end();) {
		m_AABBAllocObj->BackObj(*item);//���ȷ��ص�ǰ����Ϣ
		m_SplitAABBList->erase(item++);//ɾ����ǰ���
	}
}
ViewRange::~ViewRange()
{
	RecycleAABBPoint(PointType::BODY_TYPE);
	RecycleAABBPoint(PointType::VIEW_TYPE);
}
//�Լ��ƶ��Ļ�
void ViewRange::MoveSelf() {//֪ͨÿ���۲쵽�Լ�����,��ǰ�Լ��ƶ���
	for (auto item = m_BeObserverMap.begin(); item != m_BeObserverMap.end(); ) {
		if (!item->second->InquiryBeObserver(this))//����Լ���ǰ���ڶԷ�����Ұ��
			m_BeObserverMap.erase(item++);
		else
			item++;
	}
}

bool ViewRange::InquiryBeObserver(ViewRange* actor) {//�ƶ��Ľ�ɫѯ�ʵ�ǰ�Ƿ��ڼ�����Χ��
	bool isObserver = this->GetBodyAABB(PointType::VIEW_TYPE).Contains(actor->GetBodyAABB(PointType::BODY_TYPE));//�ж�����AABB�Ƿ��ص�
	if (!isObserver && m_ObserverMap.count(actor->m_Actor->ID()))//�����ǰ���ù۲쵽���������,�����ڿ�������
		m_ObserverMap.erase(actor->m_Actor->ID());
	return isObserver;//ɾ���Լ�
}

  
AxisDistanceManager::AxisDistanceManager() {
}
inline void AxisDistanceManager::AddtionCalcBody(ActorID id) {
	m_DelayCalcMoveList.insert(id);//�����ɫ������һ֡���ؼ���
}
inline bool AxisDistanceManager::RegisterBody(ActorID id) {
	if (m_ViewObjMap.count(id)) return false;//�����Ѿ�ע�� 
	BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//ͨ��������Ƶ�Ԫ��ȡ����Ӧ������ 
	m_ViewObjMap[id] = new ViewRange(bodyData, &m_AABBIdleGenerate);//ע����뼴��
	AddtionCalcBody(id);//����һ֡���¼����ɫ��Ϣ
	return true;
}
//���¼����ɫ����
void AxisDistanceManager::RecalcActorDistance() {
	//�������һ�µ�ǰ���ƶ�����
	MoveActor();//�������н�ɫ���Ӿ�,������ײAABB 
	CalcViewObj();//�����ɫ���Թ۲쵽����ͼ���� 
}
inline void AxisDistanceManager::RemoveDistanceAABB(ViewRange* actoViewRange, PointType type) {
	const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//��ȡ����ǰ���к��AABB
	int index = 1;
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (auto item = splitAABBList.begin(); item != splitAABBList.end(); item++, index++) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//��ȡ����ǰ������ID
		RemoveDistancePoint(actorID, type, *(*item));//ɾ��ԭ�е�������ײ����
	}
}
inline void AxisDistanceManager::AdditionDistanceAABB(ViewRange* actoViewRange, PointType type) {
	const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//��ȡ����ǰ���к��AABB
	int index = 1;
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (auto item = splitAABBList.begin(); item != splitAABBList.end(); item++, index++) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//��ȡ����ǰ������ID
		AdditionDistancePoint(actorID, type, *(*item));//ɾ��ԭ�е�������ײ����
	}
}
//��ʹ�ñ�����֮ǰӦȷ��,��ǰ���б���,���޶�Ӧ��ɫ
inline void AxisDistanceManager::AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
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
	(*xRangeRecordMap[viewRange.lowerBound.x])[addType].insert(actorID);
	(*xRangeRecordMap[viewRange.upperBound.x])[addType].insert(actorID);
	(*yRangeRecordMap[viewRange.lowerBound.y])[addType].insert(actorID);
	(*yRangeRecordMap[viewRange.upperBound.y])[addType].insert(actorID);
}
inline void AxisDistanceManager::RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //������СID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //�������ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];
	if (xRangeRecordMap.count(viewRange.lowerBound.x))//ɾ�����ID��Ӧ��X�����
		if ((*xRangeRecordMap[viewRange.lowerBound.x])[type].count(minActorID))
			(*xRangeRecordMap[viewRange.lowerBound.x])[type].erase(minActorID);
	if (xRangeRecordMap.count(viewRange.upperBound.x))//���ڵ�ǰ�����Ļ� 
		if ((*xRangeRecordMap[viewRange.upperBound.x])[type].count(maxActorID))
			(*xRangeRecordMap[viewRange.upperBound.x])[type].erase(maxActorID);
	if (yRangeRecordMap.count(viewRange.lowerBound.y))//���ڵ�ǰ�����Ļ� 
		if ((*yRangeRecordMap[viewRange.lowerBound.y])[type].count(minActorID))
			(*yRangeRecordMap[viewRange.lowerBound.y])[type].erase(minActorID);
	if (yRangeRecordMap.count(viewRange.upperBound.y))//���ڵ�ǰ�����Ļ� 
		if ((*yRangeRecordMap[viewRange.upperBound.y])[type].count(maxActorID))
			(*yRangeRecordMap[viewRange.upperBound.y])[type].erase(maxActorID);
	//�ж��Ƿ�ʹ�ý�����
	ABBBRnageRecycle(viewRange.lowerBound.x, true);
	ABBBRnageRecycle(viewRange.upperBound.x, true);
	ABBBRnageRecycle(viewRange.lowerBound.y, false);
	ABBBRnageRecycle(viewRange.upperBound.y, false);
}
inline void AxisDistanceManager::ABBBRnageRecycle(float pos, bool isX)
{
	ViewRangeRecordMap& tempMap = isX ? m_AxisBodyMap[AxisType::X_AXIS] : m_AxisBodyMap[AxisType::Y_AXIS];
	if (!tempMap.count(pos))
		return;
	ViewRangeTypeSet* rangeTypeSet = tempMap[pos];
	bool isEmpty = true;
	for (int type = 0; type < MAX_POINT_TYPE; type++) {
		if (!(*rangeTypeSet)[(PointType)type].empty()) {
			isEmpty = false;
			break;
		}
	}
	if (!isEmpty)
		return;
	m_ViewCellIdleGenerate.BackObj(rangeTypeSet);//������
	tempMap.erase(pos);
}
void AxisDistanceManager::MoveActor()
{
	//�ƶ���ɫ,�����ɫ�����ƶ�������,�������һ���Ӿ෶Χ���ж�
	ViewRange* actoViewange = NULL;
	bool OnlyCalculate = false;
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = *item;
		OnlyCalculate = true;
		if (!m_ViewObjMap.count(actorID))//���󲻴���,ֱ��ɾ��
			goto erase;
		actoViewange = m_ViewObjMap[actorID];
		if (actoViewange->JudgeActorViewAction() == 1) {//�ƶ������ǳ
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
//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
bool AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset = 0, float rightOffset = 0)//ƫ�Ƶ�Ԫ����ǰҪ��ѯ��Ϣ(����ȷ����ǰ����ѯ������ڱ���)
{
	if (0 == m_AxisBodyMap[axis].count(findKey))//���󲻴���,ֱ��ɾ�� 
		return false;
	auto axisMap = m_AxisBodyMap[axis];
	auto beginIterator = axisMap.find(findKey);//Ҳ���Ǳض�����
	std::map<float, ViewRangeTypeSet*>::reverse_iterator rBeginITerator(beginIterator);
	float rightDistance = inquiryDistance + rightOffset;
	//����ѭ����������
	for (auto item = beginIterator; item != axisMap.end() && item->first <= rightDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
	//���Ȼ�ȡ����ǰѰ�ҵ���λ��
	float leftDistance = inquiryDistance - leftOffset;
	if (leftOffset == 0) {
		goto out;
	}
	rBeginITerator++;//��ǰ�ƶ�,��������Լ�
	for (auto item = rBeginITerator; item != axisMap.rend() && item->first >= leftDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
out:
	return true;
}
//�ؼ���
inline void AxisDistanceManager::CalcViewObj()
{
	//2:֪ͨ��ǰ�۲쵽�ý�ɫ�����,��ɫ�������ƶ�,Ȼ������޸�
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = *item;
		if (!m_ViewObjMap.count(actorID))//���󲻴���,ֱ��ɾ�� 
			continue;
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->MoveSelf();//�����Լ��ƶ���
	}


	//�ƶ���ɫ,��Ϊ����
	//1:��   ��ɾ����ǰ��ɫ���ܿ������������
	//1-1:���¼���,��ͼ��Χ��,�������������Ŀ
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//���ȼ��㵱ǰ�Ĳ��� 
		ActorID actorID = *item;
		if (!m_ViewObjMap.count(actorID))//���󲻴���,ֱ��ɾ�� 
			continue;
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->ClearObserverTable();//���,Ȼ�����¼���
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB(PointType::VIEW_TYPE);
		InquiryAxisPoints(XSet, viewAABB.lowerBound.x, PointType::BODY_TYPE, AxisType::X_AXIS, viewAABB.upperBound.x - viewAABB.lowerBound.x, 0,MAX_OVERFLOW_RANGE);//��ȡ��X����б�
		InquiryAxisPoints(XSet, viewAABB.lowerBound.y, PointType::BODY_TYPE, AxisType::Y_AXIS, viewAABB.upperBound.y - viewAABB.lowerBound.y, 0, MAX_OVERFLOW_RANGE);//��ȡ��X����б� 
		//���й����Ľ�ɫͳͳ����һ�¾���

		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem];
			//����AABB
			if (obseverItem->GetBodyAABB(PointType::BODY_TYPE).Contains(actoViewange->GetBodyAABB(PointType::VIEW_TYPE)))
			{
				actoViewange->EnterView(obseverItem);
			}
		}
	}
}
void AxisDistanceManager::UnregisterBody(ActorID id)
{
	if (m_ViewObjMap.count(id))
		return; //�Ѿ�ע�����  
	ViewRange* rangeObj = m_ViewObjMap[id];
	RemoveDistanceAABB(rangeObj, PointType::BODY_TYPE);
	RemoveDistanceAABB(rangeObj, PointType::VIEW_TYPE);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}

