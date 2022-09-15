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
struct XAxisComapare {
	bool operator()(const float& xAxis1, const float& xAxis2) const {
		return xAxis1 < xAxis2; 
	}
};
class InPosActors {
public:
	InPosActors() {}
	bool operator()(const InPosActors* id1, const InPosActors* id2);
public:
	float m_PositionX;
	struct skiplist* m_SkipList;
};
struct ViewObserverState {
	ViewRange* m_ViewRange;
	bool m_IsVisible; 
};

class AxisMap{
private:
	std::map<float, struct skiplist*, XAxisComapare> m_XAxisActors[PointType::MAX_POINT_TYPE];//��ÿ��AABB����ߵ��X��������.
	std::vector<InPosActors*> m_RecordArray[PointType::MAX_POINT_TYPE];//Ϊ�˷�ֹ�ظ�����,�˷�����. ��ÿ��׼��Ѱ��ʱ,��Ҫ������map������,������������ȥ,�Լӿ�����
	AutomaticGenerator<b2Vec2> m_b2VecIdleGenerate;//�Զ���������ͼ�������Ķ��� 
	AutomaticGenerator<InPosActors> m_InPosActorsGenerate;//�Զ���������ͼ�������Ķ��� 
public:
	void AddtionViewRange(PointType type,ViewRange* actor, b2AABB& aabb);//���һ����λ
	void DeleteViewRange(PointType type, ViewRange* actor, b2AABB& aabb);//ɾ��һ����λ  
	void ReadyInitActorData();//��ʼ����λ 
	 
	int SetRangeActors(PointType type,ViewRange* actor, std::vector<ViewObserverState*>& outData, const b2AABB& range);//Ѱ���ཻ ��AABB  

	AxisMap():m_InPosActorsGenerate(200) {}
	~AxisMap();
}; 

class AxisDistanceManager;
class ViewRange{
public:
	friend class AxisMap;
	friend class AxisDistanceManager;
public:
	ViewRange(BodyData* bodyData,b2Vec2 range = b2Vec2(0.5, 0.5));//��ͼ�ĳ�ʼ��
	BodyData* GetActor();//��ǰ����ͼ����
	const b2Vec2& GetViewRange(); //��ȡ����ǰ�Ĺ۲췶Χ
	const b2Vec2& GetBodyPos();//��ȡ����ǰ��ҵĵ�λ 
	void GetObserverArray(std::vector<ViewRange*>& outData);//��ȡ�����пɼ���ɫ 
	std::vector<ViewObserverState*>& GetBeObserverArray();//��ȡ�����пɼ���ɫ 
	b2AABB& GetBodyAABB();//��ȡ����ǰ��ɫ�ĸ���AABB   
	b2AABB& GetViewAABB();//��ȡ����ǰ��ɫ�ĸ���AABB  

	bool IsRefreshActorView();//�����Ӳ�,���ڶ��ٲŻ��ؼ���  
	void RecalcBodyPosition();//���»�ȡ����ɫ��һ��δˢ�µ�λ��

	void RecalcBodyAABB();//���¼����ӿ� 
	void RecalcViewAABB();//���¼����ӿ� 
	
	void ClearExtraObserverArr();//ɾ�����ж���Ĺ۲����
	void ChangeObserverStatus(ViewRange* beObserverActor, bool status);
	void ShiftObserverPoint();//ת���۲�ָ��

	void ClearExtraBeObserverArr();//ɾ�����ж���Ĺ۲����
	void ChangeBeObserverStatus(ViewRange* beObserverActor, bool status);
	void ShiftBeObserverPoint();//ת���۲�ָ��

	void RecalcRefreshCondtion();//�ؼ���ˢ������    

	void RecycleStatusObj(std::vector<ViewObserverState*>* obj)
	{
		for (int i = 0; i < obj->size();i++)
			m_ViewObserverStateGenerate.BackObj(obj->at(i));
		obj->clear();
	}
	void RecycleStatusObj(std::unordered_map<ActorID, ViewObserverState*>& obj)
	{
		for(auto item = obj.begin();item != obj.end();item++)
			m_ViewObserverStateGenerate.BackObj(item->second); 
		obj.clear();
	}
	~ViewRange(){
		RecycleStatusObj(m_ObserverPoint);
		RecycleStatusObj(m_FrontObserverPoint);
		RecycleStatusObj(m_FrontBeObserverPoint);
		RecycleStatusObj(m_BeObserverPoint);
		RecycleStatusObj(m_ExtraBeObserverArr);
		RecycleStatusObj(m_ExtraObserverArr);
	}
private:
	BodyData* m_Actor;//��ǰ�Ľ�ɫ 
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��

	std::vector<ViewObserverState*>* m_FrontObserverPoint;//��һ��ԭʼ������һ֡���Կ�����
	std::vector<ViewObserverState*>* m_ObserverPoint;//�ڶ�������֡
	std::vector<ViewObserverState*> m_ObserverArr[2];//���Կ����Ľ�ɫ�б�.
	std::unordered_map<ActorID,ViewObserverState*> m_ExtraObserverArr;//��ֵ�����˹۲쵽��ʱ��,����ֱ�Ӳ�������,����ʹ���������

	std::vector<ViewObserverState*>* m_FrontBeObserverPoint;//��һ��ԭʼ������һ֡���Կ�����
	std::vector<ViewObserverState*>* m_BeObserverPoint;//�ڶ�������֡
	std::vector<ViewObserverState*> m_BeObserverArr[2];//���Կ����Ľ�ɫ�б�.
	std::unordered_map<ActorID, ViewObserverState*> m_ExtraBeObserverArr;//���Կ����Ľ�ɫ�б�    

	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  

	b2AABB m_AABB[PointType::MAX_POINT_TYPE];//��¼��ɫ��һ�����ʱ��AABB      
	b2Vec2 m_OffsetCondtion;//ƫ�Ƽ��� 

	AutomaticGenerator<ViewObserverState> m_ViewObserverStateGenerate;//�Զ���������ͼ�������Ķ��� 
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
	void CalcObserver(std::vector<ViewRange*>& moveRangeArr);//�ؼ��� 
	void CalcBeObserver(std::vector<ViewRange*>& moveRangeArr);//�ؼ��� 
	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager() { m_AxisBodyMap = new AxisMap; }
	~AxisDistanceManager() { delete m_AxisBodyMap; }
private:
	//��ǰ���еľ������
	AxisMap* m_AxisBodyMap;//������X�����к�Y������ 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ 

	std::map<ActorID, ViewRange*> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
	std::map<ActorID,ViewRange*> m_CalcMoveList;//Ӧ�ñ�������б�
}; 
 
inline const b2Vec2& ViewRange::GetViewRange() //��ȡ����ǰ����ͼ��Χ
{ 
	return m_ObserverRange;
}
inline void ViewRange::ShiftObserverPoint() {
	auto tempPoint = m_FrontObserverPoint;
	m_FrontObserverPoint = m_ObserverPoint;
	m_ObserverPoint = tempPoint; //�û�һ�±�֡��Ϣ  
}
inline void ViewRange::ShiftBeObserverPoint() {
	auto tempPoint = m_FrontBeObserverPoint;
	m_FrontBeObserverPoint = m_BeObserverPoint;
	m_BeObserverPoint = tempPoint; //�û�һ�±�֡��Ϣ  
} 
inline const b2Vec2& ViewRange::GetBodyPos() {//��ȡ����ǰ��ҵĵ�λ

	return m_BodyPos; 
} 
inline b2AABB& ViewRange::GetBodyAABB()//��ȡ����ǰ��ɫ�ĸ���AABB 
{
	return m_AABB[PointType::BODY_TYPE];
}
inline b2AABB& ViewRange::GetViewAABB()//��ȡ����ǰ��ɫ�ĸ���AABB 
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
  
inline bool ViewRange::IsRefreshActorView()//�����Ӳ�,���ڶ��ٲŻ��ؼ���
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//��ȡ����ǰ��λ�� 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.y - m_BodyPos.y);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return true;
	return false;
}
inline void ViewRange::GetObserverArray(std::vector<ViewRange*>& outData)//��ȡ�����пɼ���ɫ
{ 
	for (int i = 0;i < m_BeObserverPoint->size();i++){
		if (m_BeObserverPoint->at(i)->m_IsVisible == 0)
			outData.push_back(m_BeObserverPoint->at(i)->m_ViewRange); 
	} 
	int index = m_BeObserverPoint->size();
	for (auto item = m_ExtraObserverArr.begin();item != m_ExtraObserverArr.end();item++, index++) {
		if (item->second->m_IsVisible == 0)
			outData.push_back(item->second->m_ViewRange); 
	} 
}
inline std::vector<ViewObserverState*>& ViewRange::GetBeObserverArray()//��ȡ�����пɼ���ɫ
{
	return *this->m_BeObserverPoint;
}

inline void AxisDistanceManager::AddDelayCalcTable(ActorID id) { 
	if (m_DelayCalcMoveList.count(id))
		return;
	if (m_ViewObjMap.count(id) == 0)
		return; 
	m_DelayCalcMoveList[id] = m_ViewObjMap[id];
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
	m_AxisBodyMap->DeleteViewRange(PointType::BODY_TYPE, rangeObj, rangeObj->GetBodyAABB());
	m_AxisBodyMap->DeleteViewRange(PointType::VIEW_TYPE, rangeObj, rangeObj->GetViewAABB());
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
	return id1->m_PositionX < id2->m_PositionX;
}