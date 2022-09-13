#pragma once
#include "Manager/Base/BodyData.h" 
#include <map>
#include <list>      
#include <unordered_map>
#include <unordered_set>
#include <chrono> 
#include <set>
#include <algorithm>
#include <typeinfo>
#include <algorithm>
#include "Define.h"  
#include "TemplateBoard.h"  
#include "Library/JumpList/JumpList.h"
class ViewRange;
enum PointType {
	BODY_TYPE = 0,//��������
	VIEW_TYPE = 1,//��ͼ����
	MAX_POINT_TYPE = 2,
}; 
enum PointAxisType {
	LEFT_BUTTOM = 0,
	RIGHT_TOP   = 1,
	MAX_AXIS_TYPE,
};
struct b2VecCompare {
	bool operator()(const b2Vec2* k1, const b2Vec2* k2) const {
		if (k1->x != k2->x)
			return k1->x < k2->x;
		return k1->y < k2->y;
	}
};  
class InPosActors{
public:
	InPosActors(){}
	bool operator()(const InPosActors* id1, const InPosActors* id2);
public:
	b2Vec2 m_Position;
	struct skiplist* m_SkipList;
};


class AxisMap{
private: 
	std::map<b2Vec2*, struct skiplist*, b2VecCompare> m_AxisActors[PointAxisType::MAX_AXIS_TYPE];//��¼һ���Ե�λ������������ͼ����
	std::vector<InPosActors*> m_RecordArray[PointAxisType::MAX_AXIS_TYPE];//Ϊ�˷�ֹ�ظ�����,�˷�����. ��ÿ��׼��Ѱ��ʱ,��Ҫ������map������,������������ȥ,�Լӿ�����
	AutomaticGenerator<b2Vec2> m_b2VecIdleGenerate;//�Զ���������ͼ�������Ķ��� 
	AutomaticGenerator<InPosActors> m_InPosActorsGenerate;//�Զ���������ͼ�������Ķ��� 
public:
	bool AddtionViewRange(ViewRange* actor, b2AABB& aabb);//���һ����λ
	void DeleteViewRange(ViewRange* actor, b2AABB& aabb);//ɾ��һ����λ  
	void ReadyInitActorData();//��ʼ����λ 
	int GetRangeActors(std::vector<ActorID>& outData, const b2AABB& range);//Ѱ���ཻ ��AABB
	AxisMap() {
		m_RecordArray[PointAxisType::LEFT_BUTTOM].reserve(10);
		m_RecordArray[PointAxisType::RIGHT_TOP].reserve(10);
	}
	~AxisMap();
}; 

class ViewRange{
public:
	ViewRange(BodyData* bodyData,b2Vec2 range = b2Vec2(0.5, 0.5));//��ͼ�ĳ�ʼ��
	BodyData* GetActor();//��ǰ����ͼ����
	const b2Vec2& GetViewRange(); //��ȡ����ǰ�Ĺ۲췶Χ
	const b2Vec2& GetBodyPos();//��ȡ����ǰ��ҵĵ�λ 
	std::vector<ActorID>& GetObserverArray();//��ȡ�����пɼ���ɫ 
	b2AABB& GetBodyAABB();//��ȡ����ǰ��ɫ�ĸ���AABB   
	const b2AABB& GetViewAABB() const;//��ȡ����ǰ��ɫ�ĸ���AABB 
	const b2AABB& GetAABB(PointType type) const;//��ȡ����ǰ��ɫ�ĸ���AABB 
	 
	bool InObserverContain(ViewRange* actor); //�����ƶ�AABB�ǰ����Լ� 

	bool IsRefreshActorView();//�����Ӳ�,���ڶ��ٲŻ��ؼ���  
	void RecalcBodyPosition();//���»�ȡ����ɫ��һ��δˢ�µ�λ��

	void RecalcBodyAABB();//���¼����ӿ� 
	void RecalcViewAABB();//���¼����ӿ� 

	void RecalcRefreshCondtion();//�ؼ���ˢ������    

	void GetObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor);//Ѱ���Լ��ӿڵ�ĳһ������ 
	 
	~ViewRange() {}
private:
	BodyData* m_Actor;//��ǰ�Ľ�ɫ 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��

	std::vector<ActorID> m_ObserverArr;//���Կ����Ľ�ɫ�б�    

	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  

	b2AABB m_AABB[PointType::MAX_POINT_TYPE];//��¼��ɫ��һ�����ʱ��AABB      
	b2Vec2 m_OffsetCondtion;//ƫ�Ƽ��� 
}; 

class AxisDistanceManager
{
public:
	//ע���б�
	bool RegisterBody(BodyData* actor);//ע��һ�����󵽾������
	void UnregisterBody(ActorID id);//��ע��
	void UnregisterBody(BodyData* actor);//��ע��
	//�������� 
	void AddDelayCalcTable(ActorID id);//���һ������������  
	//��ȡ����Ԫ
	ViewRange* GetViewRange(ActorID actorID);
	//�ؼ���
	void Update();//���¼����ɫ����
	void ActorsMove();
	void CalcViewObj();//�ؼ��� 
	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager() {} 
private:
	//��ǰ���еľ������
	AxisMap m_AxisBodyMap;//������X�����к�Y������ 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ 

	std::unordered_map<ActorID, ViewRange*> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
	std::vector<ViewRange*> m_CalcMoveList;//Ӧ�ñ�������б�
}; 
 
inline const b2Vec2& ViewRange::GetViewRange() //��ȡ����ǰ����ͼ��Χ
{ 
	return m_ObserverRange;
}
inline const b2Vec2& ViewRange::GetBodyPos() {//��ȡ����ǰ��ҵĵ�λ

	return m_BodyPos; 
} 
inline b2AABB& ViewRange::GetBodyAABB()//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_AABB[PointType::BODY_TYPE];
}
inline const b2AABB& ViewRange::GetViewAABB() const//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_AABB[PointType::VIEW_TYPE];//��ȡ��ĳһ���͵�AABB����
}

inline void ViewRange::RecalcViewAABB()//���¼����ӿ�
{
	m_AABB[PointType::VIEW_TYPE].lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);//���¼�����ͼ
	m_AABB[PointType::VIEW_TYPE].upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
}
inline void ViewRange::RecalcBodyAABB()//���¼����ӿ�
{ 
	m_Actor->CalcBodyAABB(); 
	memcpy(&m_AABB[PointType::BODY_TYPE], &m_Actor->GetAABB(), sizeof(b2AABB));//��������
	RecalcRefreshCondtion(); 
} 

inline void ViewRange::RecalcBodyPosition()
{ 
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}
inline void ViewRange::RecalcRefreshCondtion()//�ؼ���ˢ������
{	//��ɫ�ƶ�������ڽ�ɫ��ǰ�ٷ�֮10��λ�õĻ�
	m_OffsetCondtion.x = (m_AABB[PointType::BODY_TYPE].upperBound.x - m_AABB[PointType::BODY_TYPE].lowerBound.x) * 0.2f;
	m_OffsetCondtion.y = (m_AABB[PointType::BODY_TYPE].upperBound.y - m_AABB[PointType::BODY_TYPE].lowerBound.y) * 0.2f;
}
inline BodyData* ViewRange::GetActor() {//��ȡ����ǰ�Ľ�ɫ
	return m_Actor;
}

inline bool ViewRange::InObserverContain(ViewRange* actor) {//�ƶ��Ľ�ɫѯ�ʵ�ǰ�Ƿ��ڼ�����Χ��  
	auto startItor = std::lower_bound(m_ObserverArr.begin(), m_ObserverArr.end(),actor->GetActor()->ID());
	return startItor != m_ObserverArr.end() && *startItor == actor->GetActor()->ID(); 
}
inline const b2AABB& ViewRange::GetAABB(PointType type) const//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_AABB[type];
}
inline bool ViewRange::IsRefreshActorView()//�����Ӳ�,���ڶ��ٲŻ��ؼ���
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.y - m_BodyPos.y);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return true;
	return false;
}
inline std::vector<ActorID>& ViewRange::GetObserverArray()//��ȡ�����пɼ���ɫ
{
	return this->m_ObserverArr;
}
  

inline void AxisDistanceManager::AddDelayCalcTable(ActorID id) { 
	if (m_DelayCalcMoveList.count(id))
		return;
	if (m_ViewObjMap.count(id) == 0)
		return; 
	m_DelayCalcMoveList[id] = m_ViewObjMap[id];//�����ɫ������һ֡���ؼ���
}
inline bool AxisDistanceManager::RegisterBody(BodyData* actor) {
	ActorID actorID = actor->ID();
	if (m_ViewObjMap.count(actorID)) return false;//�����Ѿ�ע��  
	m_ViewObjMap[actorID] = new ViewRange(actor);//ע����뼴��
	AddDelayCalcTable(actorID);//����һ֡���¼����ɫ��Ϣ
	return true;
}

inline void AxisDistanceManager::UnregisterBody(ActorID id)
{
	if (!m_ViewObjMap.count(id)) return;//δע���
	ViewRange* rangeObj = m_ViewObjMap[id];//��ȡ������ 
	m_AxisBodyMap.DeleteViewRange(id, rangeObj->GetBodyAABB());
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


inline void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
//���¼����ɫ����
inline void AxisDistanceManager::Update() {
	//��������ƶ����Ľ�ɫ���¼������ǵĿ��ӷ�Χ�ڵ����� 

	int64_t moveCost;
	int64_t calcCost;
	auto t1 = std::chrono::high_resolution_clock::now();
	ActorsMove();//�������н�ɫ���Ӿ�,������ײAABB  
	auto t2 = std::chrono::high_resolution_clock::now();
	moveCost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "������ĵ�λ��:" << m_CalcMoveList.size() << std::endl;
	t1 = std::chrono::high_resolution_clock::now();
	CalcViewObj();//�����ɫ���Թ۲쵽����ͼ����  
	t2 = std::chrono::high_resolution_clock::now();
	calcCost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count(); 
	std::cout << "�ƶ���ʱ" << moveCost << " �����ʱ:" << calcCost << "�ܺ�ʱ:" << moveCost + calcCost << std::endl;
}
  
inline AxisDistanceManager& AxisDistanceManager::Instance() {
	static AxisDistanceManager Instance;
	return Instance;
}
inline ViewRange* AxisDistanceManager::GetViewRange(ActorID actorID)
{
	return m_ViewObjMap[actorID];
}

inline bool InPosActors::operator()(const InPosActors* id1, const InPosActors* id2) {
	if (id1->m_Position.x != id2->m_Position.x)
		return id1->m_Position.x < id2->m_Position.x;
	return id1->m_Position.y < id2->m_Position.y;
}