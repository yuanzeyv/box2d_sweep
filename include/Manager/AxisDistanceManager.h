#pragma once
#include "Manager/Base/BodyData.h" 
#include <map>
#include <list>      
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <typeinfo>
#include <algorithm>
#include "Define.h" 
#include "TemplateBoard.h" 
//bit 50-64 ʣ������
//bit 48-49 ����ID
//bit 00-47  ��ɫID
#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ((ActorID)type) << 48) //���������С������
#define GET_ACTOR_ID(actorID) (actorID & (~0xFFFF000000000000))//��ȡ����ǰ�Ľ�ɫID 
enum PointType {
	BODY_TYPE = 0,
	VIEW_TYPE = 1,
	MAX_POINT_TYPE = 2,
};
enum AxisType {
	X_AXIS = 0,
	Y_AXIS = 1,
	MAX_AXIS_TYPE ,
};  
class OrderlyB2Vec2Array{//��X�� Y��������
public:
	bool operator()(const b2Vec2& k1, const b2Vec2& k2) const {
		if (k1.x != k2.x)
			return k1.x < k2.x; 
		return k1.y < k2.y;
	}
	inline bool IsExist(ActorID actorID)
	{
		std::vector<ActorID>::iterator itor = lower_bound(m_ActorArr.begin(), m_ActorArr.end(), actorID);//�����ҵ���ǰԪ��
		return *itor == actorID;//�Ƿ��Ѿ�������
	}
	inline bool Insert(ActorID actorID) { 
		std::vector<ActorID>::iterator itor = lower_bound(m_ActorArr.begin(), m_ActorArr.end(), actorID);//�����ҵ���ǰԪ��
		m_ActorArr.insert(itor,actorID);//��ָ��λ�ò���
	} 
	//�������ظ�
	bool Merge(std::vector<ActorID>& actorArr) {
		//����ʹ�ö��ֲ���,�ҵ���һ�������λ��,Ȼ��ӵ�ǰλ�ÿ�ʼ����
		lower_bound()
		merge()
	}
private:
	std::vector<ActorID> m_ActorArr;
};  
typedef std::map<b2Vec2, std::vector<ActorID>, OrderlyB2Vec2Array > ViewRangeRecordMap;//ĳһ����λ��,������ĳһ���ɫ����

//��ǰ����ͼ��Χ
class ViewRange{
public:
	ViewRange(BodyData* bodyData,b2Vec2 range = b2Vec2(0.5, 0.5));//��ͼ�ĳ�ʼ��
	BodyData* GetActor(); //��ȡ����ǰ�Ľ�ɫ 
	const b2Vec2& GetViewRange(); //��ȡ����ǰ�Ĺ۲췶Χ
	const b2Vec2& GetBodyPos();//��ȡ����ǰ��ҵĵ�λ 
	const std::unordered_map<ActorID, ViewRange*>& GetVisibleMap();//��ȡ�����пɼ���ɫ
	const std::unordered_map<ActorID, ViewRange*>& GetBeObseverView(ViewRange* actor);//ĳһ����ɫ���뵽�˱��۲����ͼ
	const b2AABB& GetBodyAABB() const;//��ȡ����ǰ��ɫ�ĸ���AABB   
	const b2AABB& GetViewAABB() const;//��ȡ����ǰ��ɫ�ĸ���AABB 
	const b2AABB& GetAABB(PointType type) const;//��ȡ����ǰ��ɫ�ĸ���AABB 

	bool EnterView(ViewRange* actor);//ĳһ����ɫ������Ұ 
	void LeaveView(ActorID actorID); //ĳһ����ɫ�뿪����ͼ 
	bool EnterBeObseverView(ViewRange* actor);//ĳһ����ɫ���뵽�˱��۲����ͼ

	bool InObserverContain(ViewRange* actor); //�����ƶ�AABB�ǰ����Լ�
	void ClearObserverTable();//����ǰ�۲쵽�����н�ɫ

	int IsRefreshActorView();//�����Ӳ�,���ڶ��ٲŻ��ؼ��� 
	void MoveSelf(); //֪ͨÿ���۲쵽�Լ�����,��ǰ�Լ��ƶ���  
	void RecalcBodyPosition();//���»�ȡ����ɫ��һ��δˢ�µ�λ��

	void RecalcBodyAABB();//���¼����ӿ� 
	void RecalcViewAABB();//���¼����ӿ�
	void RecalcAABB(PointType type);//���¼����ӿ�
	void RecalcRefreshCondtion();//�ؼ���ˢ������    

	void GetObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor);//Ѱ���Լ��ӿڵ�ĳһ������
	void GetBeObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor);//Ѱ���Լ��ӿڵ�ĳһ������

	static bool Compare(ViewRange*& v1, ViewRange*& v2);
	~ViewRange() {}
private:
	BodyData* m_Actor;//��ǰ�Ľ�ɫ 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��

	//�洢��һ�������,�Խ�ɫ�±�Ϊ���������  
	std::vector<ViewRange*> m_ObserverMap;
	std::vector<ViewRange*> m_BeObserverMap;
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
	void AdditionAABB(PointType type,ViewRange* actoViewRange);//���һ����ͼAABB
	void RemoveAABB(PointType type,ViewRange* actoViewRange);
	//��ȡ����Ԫ
	ViewRange* GetViewRange(ActorID actorID);
	//�ؼ���
	void Update();//���¼����ɫ����
	void ActorsMove();
	void CalcViewObj();//�ؼ���

	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager() {}
	//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
	void InquiryAxisPoints(std::unordered_set<ActorID>& outData,const b2AABB& viewAABB);
private:
	//�ڴ������� 
	AutomaticGenerator<SmartActorIDMan> m_ViewCellIdleGenerate;//�Զ���������ͼ�������Ķ��� 
	//��ǰ���еľ������
	ViewRangeRecordMap m_AxisBodyMap[PointType::MAX_POINT_TYPE][AxisType::MAX_AXIS_TYPE];//������X�����к�Y������  
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ 

	std::unordered_map<ActorID, ViewRange*> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
	std::unordered_map<ActorID, ViewRange*> m_CalcMoveList;//Ӧ�ñ�������б�
};
bool ViewRange::Compare(ViewRange*& v1, ViewRange*& v2) {
	return v1->GetActor()->ID() < v2->GetActor()->ID();//ID��С��������
}
inline void ViewRange::GetObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor) {//Ѱ���Լ��ӿڵ�ĳһ������
	auto iterEnd = m_ObserverMap.end();
	itor = std::lower_bound(m_ObserverMap.begin(), iterEnd, actor, ViewRange::Compare);
	if (itor == iterEnd || (*itor)->GetActor()->ID() != actor->GetActor()->ID())
		itor = iterEnd;
}
inline void ViewRange::GetBeObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor) {//Ѱ���Լ��ӿڵ�ĳһ������
	auto iterEnd = m_ObserverMap.end();
	itor = std::lower_bound(m_ObserverMap.begin(), iterEnd, actor, ViewRange::Compare); 
	if (itor == iterEnd || (*itor) == actor)
		itor = iterEnd;
}

inline void ViewRange::ClearObserverTable() {  //����ǰ�۲쵽�����н�ɫ
	m_ObserverMap.clear();
}

inline const b2Vec2& ViewRange::GetViewRange() { //��ȡ����ǰ����ͼ��Χ
	return m_ObserverRange;
}
inline const b2Vec2& ViewRange::GetBodyPos() {//��ȡ����ǰ��ҵĵ�λ

	return m_BodyPos;
}
inline void ViewRange::LeaveView(ActorID actorID) //ĳһ����ɫ�뿪����ͼ
{ 

	m_ObserverMap.erase(actorID);
}
inline const b2AABB& ViewRange::GetBodyAABB() const//��ȡ����ǰ��ɫ�ĸ���AABB 
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
inline void ViewRange::RecalcAABB(PointType type)//���¼����ӿ�
{
	if (type == PointType::BODY_TYPE)
		RecalcBodyAABB();
	else if (type == PointType::VIEW_TYPE)
		RecalcViewAABB();
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
	if (!b2TestOverlap(m_AABB[PointType::VIEW_TYPE], actor->GetBodyAABB())) {//�����ǰ���ù۲쵽���������,�����ڿ�������
		m_ObserverMap.erase(actor->m_Actor->ID());//ִ��ɾ��
		return false;
	}
	return true;//ɾ���Լ�
}
inline const b2AABB& ViewRange::GetAABB(PointType type) const//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_AABB[type];
}
inline int ViewRange::IsRefreshActorView()//�����Ӳ�,���ڶ��ٲŻ��ؼ���
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.y - m_BodyPos.y);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return 1;
	return 0;
}
inline bool ViewRange::EnterView(ViewRange* actor)//ĳһ����ɫ������Ұ
{
	m_ObserverMap[actor->GetActor()->ID()] = actor;//�ҹ۲쵽�˱���
	actor->EnterBeObseverView(this);//���˱��ҹ۲쵽��
	return true;
}
inline const std::unordered_map<ActorID, ViewRange*>& ViewRange::GetVisibleMap()//��ȡ�����пɼ���ɫ
{
	return this->m_ObserverMap;
}

inline bool ViewRange::EnterBeObseverView(ViewRange* actor)//ĳһ����ɫ���뵽�˱��۲����ͼ
{
	if (m_BeObserverMap.count(actor->GetActor()->ID()))//���ھͲ�����
		return true;
	m_BeObserverMap[actor->GetActor()->ID()] = actor;
	return true;
}

inline const std::unordered_map<ActorID, ViewRange*>& ViewRange::GetBeObseverView(ViewRange* actor)//ĳһ����ɫ���뵽�˱��۲����ͼ
{
	return m_BeObserverMap;
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
	RemoveAABB(PointType::BODY_TYPE,rangeObj);
	RemoveAABB(PointType::BODY_TYPE, rangeObj);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


inline void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
//���¼����ɫ����
inline void AxisDistanceManager::Update() {
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