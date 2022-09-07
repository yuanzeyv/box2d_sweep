#pragma once
#include "Manager/Base/BodyData.h" 
#include <map>
#include <list>     
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include "Define.h" 
#include "TemplateBoard.h"  
//bit 50-64 ʣ������
//bit 48-49 ����ID
//bit 00-47  ��ɫID
#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ((type & 3) << 47)) //���������С������
#define GET_ACTOR_ID(actorID) (actorID & (~((ActorID)(0xffff) << 47)))//��ȡ����ǰ�Ľ�ɫID
enum PointPosType {//Ҫ��ҪӦ�ö�����
	POS_BODY_LIMIT_LEFT_MAX = 0,//����
	POS_BODY_LIMIT_LEFT_MIN = 1,//���� 
	POS_BODY_LIMIT_RIGHT_MAX = 2,//���� 
	POS_BODY_LIMIT_RIGHT_MIN = 3,//���� 
	POS_BODY_LIMIT_NUM_MAX = 4,//������
}; 
enum PointType {
	BODY_TYPE = 0,
	VIEW_TYPE = 1,
	MAX_POINT_TYPE = 2,
};
struct CompareFloat {
	bool operator()(const b2Vec2& k1, const b2Vec2& k2) const {
		if (k1.x < k2.x)
			return true;
		if (k1.x > k2.x)
			return false;
		return k1.y < k2.y;
	}
}; 
class SmartActorIDMan {
public:
	inline void Insert(ActorID actorID, PointPosType type)
	{
		actorID = GEN_AABB_POINT_TYPE(actorID, type);//����һ���µ�
		m_ActorSet.insert(actorID);
	}
	inline void Erease(ActorID actorID, PointPosType type)
	{
		actorID = GEN_AABB_POINT_TYPE(actorID, type);//����һ���µ�
		m_ActorSet.erase(actorID);
	}
	inline const std::unordered_set<ActorID>& GetTable() {
		return  m_ActorSet;
		
	}
	inline void Init() {
		m_ActorSet.clear();
	}
	inline bool IsInValid() { 
		return m_ActorSet.size() == 0;
	}
private: 
	std::unordered_set<ActorID> m_ActorSet;//��¼һ������Ľ�ɫID
};
typedef std::map<b2Vec2, SmartActorIDMan*, CompareFloat > ViewRangeRecordMap;//��ͼ��

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
	void RecalcRefreshCondtion();//�ؼ���ˢ������    
	~ViewRange() {}
private:
	BodyData* m_Actor;//��ǰ�Ľ�ɫ 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��

	std::unordered_map<ActorID, ViewRange*> m_ObserverMap;//���Կ����Ľ�ɫ�б�  
	std::unordered_map<ActorID, ViewRange*> m_BeObserverMap;//���ڼ�¼ÿ����ɫ��˭�۲쵽 
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  

	b2AABB m_BodyAABB;//��¼��ɫ��һ�����ʱ��AABB    
	b2AABB m_ViewAABB;//��¼��ɫ��һ�����ʱ��AABB      

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
	void AdditionViewAABB(ViewRange* actoViewRange);//���һ����ͼAABB
	void RemoveViewAABB(ViewRange* actoViewRange); 
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
	ViewRangeRecordMap m_AxisBodyMap;//������X�����к�Y������ 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ 
	std::unordered_set<ActorID> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
}; 

inline void ViewRange::ClearObserverTable() //����ǰ�۲쵽�����н�ɫ
{  
	m_ObserverMap.clear();
}

inline const b2Vec2& ViewRange::GetViewRange() //��ȡ����ǰ����ͼ��Χ
{
	return m_ObserverRange;
}
inline const b2Vec2& ViewRange::GetBodyPos()//��ȡ����ǰ��ҵĵ�λ
{
	return m_BodyPos;
}
inline void ViewRange::LeaveView(ActorID actorID) //ĳһ����ɫ�뿪����ͼ
{
	m_ObserverMap.erase(actorID);
}
inline const b2AABB& ViewRange::GetBodyAABB() const//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_BodyAABB;//��ȡ��ĳһ���͵�AABB����
}
inline const b2AABB& ViewRange::GetViewAABB() const//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_ViewAABB;//��ȡ��ĳһ���͵�AABB����
}

inline void ViewRange::RecalcViewAABB()//���¼����ӿ�
{
	m_ViewAABB.lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);//���¼�����ͼ
	m_ViewAABB.upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
}
inline void ViewRange::RecalcBodyAABB()//���¼����ӿ�
{
	this->m_Actor->CalcBodyAABB();
	const b2AABB& bodyAABB = m_Actor->GetAABB();//��ȡ����ɫ��AABB
	memcpy(&m_BodyAABB, &bodyAABB, sizeof(b2AABB));//��������
	RecalcRefreshCondtion(); 
}

inline void ViewRange::RecalcBodyPosition()
{ 
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}
inline void ViewRange::RecalcRefreshCondtion()//�ؼ���ˢ������
{	//��ɫ�ƶ�������ڽ�ɫ��ǰ�ٷ�֮10��λ�õĻ�
	m_OffsetCondtion = m_BodyAABB.upperBound - m_BodyAABB.lowerBound; 
	m_OffsetCondtion *= 0.1;
}
inline BodyData* ViewRange::GetActor() {//��ȡ����ǰ�Ľ�ɫ
	return m_Actor;
}

inline bool ViewRange::InObserverContain(ViewRange* actor) {//�ƶ��Ľ�ɫѯ�ʵ�ǰ�Ƿ��ڼ�����Χ�� 
	if (!b2TestOverlap(m_ViewAABB, actor->m_BodyAABB)) {//�����ǰ���ù۲쵽���������,�����ڿ�������
		m_ObserverMap.erase(actor->m_Actor->ID());//ִ��ɾ��
		return false;
	}
	return true;//ɾ���Լ�
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
	m_DelayCalcMoveList.insert(id);//�����ɫ������һ֡���ؼ���
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
	RemoveViewAABB( rangeObj);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


inline void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
//���¼����ɫ����
inline void AxisDistanceManager::Update() {
	ActorsMove();//�������н�ɫ���Ӿ�,������ײAABB  
	CalcViewObj();//�����ɫ���Թ۲쵽����ͼ����  
}
  
inline AxisDistanceManager& AxisDistanceManager::Instance() {
	static AxisDistanceManager Instance;
	return Instance;
}
inline ViewRange* AxisDistanceManager::GetViewRange(ActorID actorID)
{
	return m_ViewObjMap[actorID];
}