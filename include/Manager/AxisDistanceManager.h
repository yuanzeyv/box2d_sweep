#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <map>
#include <list>     
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include "Define.h" 
#include "TemplateBoard.h"
#define MAX_OVERFLOW_RANGE (0.5f) //����ָ����,Ҳ���ڼ��
//bit 56-64 ʣ������
//bit 48-55 ����ID
//bit 00-47  ��ɫID
#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ( (type & 1) << 56)) //����һ�����õ�λ���͵����� 
#define GEN_AABB_ID(actorID,index) (actorID | ((index & 0xffff) << 48 ))//����һ��ID

#define GET_ACTOR_ID(actorID) (actorID & (~0xffff))//��ȡ����ǰ�Ľ�ɫID
enum PointPosType {//Ҫ��ҪӦ�ö�����
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
struct ActorComapre {
	bool operator()(ActorID actorIDA, ActorID actorIDB) {
		return actorIDA < actorIDB;
	}
};
class ViewRangeTypeSet {
public:
	std::unordered_set<ActorID, ActorComapre> m_TypeSet[PointType::MAX_POINT_TYPE];
	ViewRangeTypeSet() { 
	}
	inline std::unordered_set<ActorID,ActorComapre>& operator[](PointType type) {
		return m_TypeSet[type];
	}
};
typedef std::map<float, ViewRangeTypeSet*> ViewRangeRecordMap;

   

//��ǰ����ͼ��Χ
class ViewRange{
public:
	ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range = b2Vec2(0.5, 0.5));//��ͼ�ĳ�ʼ��
	inline BodyData* GetActor(); //��ȡ����ǰ�Ľ�ɫ 
	inline const b2Vec2& GetViewRange(); //��ȡ����ǰ�Ĺ۲췶Χ
	inline const b2Vec2& GetBodyPos();//��ȡ����ǰ��ҵĵ�λ 
	inline const std::unordered_map<ActorID, ViewRange*>& GetVisibleMap();//��ȡ�����пɼ���ɫ

	inline bool EnterView(ViewRange* actor);//ĳһ����ɫ������Ұ 
	inline void LeaveView(ActorID actorID); //ĳһ����ɫ�뿪����ͼ 
	inline bool EnterBeObseverView(ViewRange* actor);//ĳһ����ɫ���뵽�˱��۲����ͼ

	inline bool IsContain(ViewRange* actor); //�����ƶ�AABB�ǰ����Լ�
	inline void ClearObserverTable();//����ǰ�۲쵽�����н�ɫ

	inline int IsRefreshActorView();//�����Ӳ�,���ڶ��ٲŻ��ؼ��� 
	inline void MoveSelf(); //֪ͨÿ���۲쵽�Լ�����,��ǰ�Լ��ƶ���  
	inline void RecalcBodyPosition();//���»�ȡ����ɫ��һ��δˢ�µ�λ��

	inline const b2AABB& GetBodyAABB(PointType type) const;//��ȡ����ǰ��ɫ�ĸ���AABB  
	inline const std::list<b2AABB*>& GetSplitAABB(PointType type);//��ȡ����ǰ�Ĳü�AABB 
	inline void RecalcAABBRange(PointType type);//���¼����ӿ� 
	inline void RecalcBodyAABB(PointType type);//���¼����ӿ�
	inline void RecalcRefreshCondtion();//�ؼ���ˢ������  
	inline void CalcSplitAABB(PointType type);//����ָ����(AABB�ᾭ����,��ͼ��������)
	inline void RecycleAABBPoint(PointType type);//���յ�λ 
	~ViewRange(); 
private:
	BodyData* m_Actor;//��ǰ�Ľ�ɫ 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��

	std::unordered_map<ActorID, ViewRange*> m_ObserverMap;//���Կ����Ľ�ɫ�б�  
	std::unordered_map<ActorID, ViewRange*> m_BeObserverMap;//���ڼ�¼ÿ����ɫ��˭�۲쵽 
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  

	b2AABB m_BodyAABB[PointType::MAX_POINT_TYPE];//��¼��ɫ��һ�����ʱ��AABB     
	std::list<b2AABB*> m_SplitAABBList[PointType::MAX_POINT_TYPE];//���յ�,�ָ���AABB

	b2Vec2 m_OffsetCondtion;//ƫ�Ƽ���

	AutomaticGenerator<b2AABB>* m_AABBAllocObj;//����һ�·���AABB�Ķ���
}; 

class AxisDistanceManager
{
public:
	//ע���б�
	inline bool RegisterBody(BodyData* actor);//ע��һ�����󵽾������
	inline void UnregisterBody(ActorID id);//��ע��
	inline void UnregisterBody(BodyData* actor);//��ע��
	//�ؼ���
	void RecalcActorDistance();//���¼����ɫ����
	inline void ActorsMove();
	inline void CalcViewObj();//�ؼ���
	//�������� 
	inline void AddDelayCalcTable(ActorID id);//���һ������������ 
	inline void AdditionDistanceAABB(ViewRange* actoViewRange, PointType type);
	inline void RemoveDistanceAABB(ViewRange* actoViewRange, PointType type);
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange);
	inline void RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange); 
	inline void ABBBRnageRecycle(float pos, ViewRangeRecordMap& axisMap);//����AABB��Ԫ
	//��ȡ����Ԫ
	inline ViewRange* GetViewRange(ActorID actorID);

	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager();
	//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
	bool InquiryAxisPoints(std::unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset = 0, float rightOffset = 0);//ƫ�Ƶ�Ԫ����ǰҪ��ѯ����Ϣ(����ȷ����ǰ����ѯ������ڱ���)
private:
	//�ڴ�������
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //�Զ���B2AABB�������
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//�Զ���������ͼ�������Ķ��� 
	//��ǰ���еľ������
	ViewRangeRecordMap m_AxisBodyMap[AxisType::MAX_AXIS];//������X�����к�Y������ 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ 
	std::unordered_set<ActorID> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
}; 



void ViewRange::ClearObserverTable() //����ǰ�۲쵽�����н�ɫ
{
	m_ObserverMap.clear();
}

const b2Vec2& ViewRange::GetViewRange() //��ȡ����ǰ����ͼ��Χ
{
	return m_ObserverRange;
}
const b2Vec2& ViewRange::GetBodyPos()//��ȡ����ǰ��ҵĵ�λ
{
	return m_BodyPos;
}
void ViewRange::LeaveView(ActorID actorID) //ĳһ����ɫ�뿪����ͼ
{
	m_ObserverMap.erase(actorID);
}
const b2AABB& ViewRange::GetBodyAABB(PointType type) const//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_BodyAABB[type];//��ȡ��ĳһ���͵�AABB����
}
const std::list<b2AABB*>& ViewRange::GetSplitAABB(PointType type)//��ȡ����ǰ�Ĳü�AABB
{
	return m_SplitAABBList[type];
}

void ViewRange::RecalcBodyAABB(PointType type)//���¼����ӿ�
{
	RecalcAABBRange(type);//���������AABB
	CalcSplitAABB(type);//�����и���AABB
}

void ViewRange::RecalcBodyPosition()
{
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}
void ViewRange::RecalcRefreshCondtion()//�ؼ���ˢ������
{	//��ɫ�ƶ�������ڽ�ɫ��ǰ�ٷ�֮10��λ�õĻ�
	m_OffsetCondtion = m_BodyAABB[PointType::BODY_TYPE].upperBound - m_BodyAABB[PointType::BODY_TYPE].lowerBound;
	m_OffsetCondtion *= 0.1;
}
BodyData* ViewRange::GetActor() {//��ȡ����ǰ�Ľ�ɫ
	return m_Actor;
}

int ViewRange::IsRefreshActorView()//�����Ӳ�,���ڶ��ٲŻ��ؼ���
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return 1;
	return 0;
}
bool ViewRange::EnterView(ViewRange* actor)//ĳһ����ɫ������Ұ
{
	m_ObserverMap[actor->GetActor()->ID()] = actor;//�ҹ۲쵽�˱���
	actor->EnterBeObseverView(this);//���˱��ҹ۲쵽��
}
const std::unordered_map<ActorID, ViewRange*>& ViewRange::GetVisibleMap()//��ȡ�����пɼ���ɫ
{
	return this->m_ObserverMap;
}

bool ViewRange::EnterBeObseverView(ViewRange* actor)//ĳһ����ɫ���뵽�˱��۲����ͼ
{
	m_BeObserverMap[actor->GetActor()->ID()] = actor;
}

void ViewRange::RecycleAABBPoint(PointType type)//���յ�λ
{
	for (auto item = m_SplitAABBList->begin(); item != m_SplitAABBList->end();) {
		m_AABBAllocObj->BackObj(*item);//���ȷ��ص�ǰ����Ϣ
		m_SplitAABBList->erase(item++);//ɾ����ǰ���
	}
}

void AxisDistanceManager::AddDelayCalcTable(ActorID id) {
	m_DelayCalcMoveList.insert(id);//�����ɫ������һ֡���ؼ���
}

void AxisDistanceManager::UnregisterBody(ActorID id)
{
	if (!m_ViewObjMap.count(id)) return;//δע���
	ViewRange* rangeObj = m_ViewObjMap[id];//��ȡ������
	RemoveDistanceAABB(rangeObj, PointType::BODY_TYPE);//ɾ������ǰ��AABb��Ϣ
	RemoveDistanceAABB(rangeObj, PointType::VIEW_TYPE);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
bool AxisDistanceManager::RegisterBody(BodyData* actor) {
	ActorID actorID = actor->ID();
	if (m_ViewObjMap.count(actorID)) return false;//�����Ѿ�ע��  
	m_ViewObjMap[actorID] = new ViewRange(actor, &m_AABBIdleGenerate);//ע����뼴��
	AddDelayCalcTable(actorID);//����һ֡���¼����ɫ��Ϣ
	return true;
}
//���¼����ɫ����
void AxisDistanceManager::RecalcActorDistance() {
	ActorsMove();//�������н�ɫ���Ӿ�,������ײAABB 
	CalcViewObj();//�����ɫ���Թ۲쵽����ͼ���� 
}

void AxisDistanceManager::RemoveDistanceAABB(ViewRange* actoViewRange, PointType type){
	int index = 1;
	auto & splitAABBList = actoViewRange->GetSplitAABB(type);//��ȡ����ǰ���к��AABB
	ActorID actorID = actoViewRange->GetActor()->ID();//��ȡ����ɫ��ID
	for (auto aabbItem = splitAABBList.begin(); aabbItem != splitAABBList.end(); aabbItem++ ) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//��ȡ����ǰ������ID
		b2AABB* aabb = *aabbItem;
		RemoveDistancePoint(actorID, type,*aabb);//ɾ��ԭ�е�������ײ����
		index++;
	}
}
void AxisDistanceManager::AdditionDistanceAABB(ViewRange* actoViewRange, PointType type) {
	const std::list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(type);//��ȡ����ǰ���к��AABB
	int index = 1;
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (auto item = splitAABBList.begin(); item != splitAABBList.end(); item++, index++) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//��ȡ����ǰ������ID
		AdditionDistancePoint(actorID, type, *(*item));//ɾ��ԭ�е�������ײ����
	}
}
void AxisDistanceManager::ABBBRnageRecycle(float pos, ViewRangeRecordMap& axisMap)
{
	if (!axisMap.count(pos)) return;//�����ڵĻ�
	ViewRangeTypeSet* rangeTypeSet = axisMap[pos];
	for (int type = 0; type < MAX_POINT_TYPE; type++)//ѭ��������ͼ��λ �� ����λ�Ƿ�Ϊ��
		if (!(*rangeTypeSet)[(PointType)type].empty())
			return;
	m_ViewCellIdleGenerate.BackObj(rangeTypeSet);//������
	axisMap.erase(pos);
}

inline AxisDistanceManager& AxisDistanceManager::Instance() {
	static AxisDistanceManager Instance;
	return Instance;
}
ViewRange* AxisDistanceManager::GetViewRange(ActorID actorID)
{
	return m_ViewObjMap[actorID];
}