#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <map>
#include <list>     
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include "Define.h"     
#define MAX_OVERFLOW_RANGE (0.5f) //����1�׵ȷ� 
//bit 56-64 ʣ������
//bit 48-55 ����ID
//bit 0-54  ��ɫID
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

class ViewRangeTypeSet {
public:
	std::unordered_set<ActorID> m_TypeSet[PointType::MAX_POINT_TYPE];
	ViewRangeTypeSet() {}
	inline std::unordered_set<ActorID>& operator[](PointType type) {
		return m_TypeSet[type];
	}
};
typedef std::map<float, ViewRangeTypeSet*> ViewRangeRecordMap;

   
template<typename ListType>
class AutomaticGenerator {
public:
	AutomaticGenerator(int allocStep = 100) :m_AllocStep(allocStep), m_AllocCount(0), m_IdleList() {}
	AutomaticGenerator::~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size()) 
			printf("��ǰ��%s����%lld��δ������,������ڴ�й©,��������߼�\n\r", typeid(ListType).name(), m_AllocCount - m_IdleList.size());
		for (auto item = m_IdleList.begin(); item != m_IdleList.end(); item++)	delete* item;
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //ȫ������
	}
	ListType* RequireObj() { 
		if (m_IdleList.size() == 0)//�б���˵Ļ�
			GenerateIdelListTypes();//1000����ɫ  
		auto beginObj = m_IdleList.begin();//��ȡ����һ��
		ListType* listTypeObj = *beginObj;//��ȡ����һ����ֵ
		m_IdleList.erase(beginObj);//ɾ����һ��
		return listTypeObj;
	}
	void BackObj(ListType* obj) {//�黹һ��ָ��
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++)
			m_IdleList.push_back(new ListType());//������ô������ж���
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//���䲽��
	int m_AllocCount;//����ĸ���
	std::std::list<ListType*> m_IdleList;//��ǰ���еĸ���
};

//��ǰ����ͼ��Χ
class ViewRange{
public:
	ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range = b2Vec2(0.5, 0.5));//��ͼ�ĳ�ʼ��
	inline BodyData* GetActor(); //��ȡ����ǰ�Ľ�ɫ 
	inline const b2Vec2& GetViewRange(); //��ȡ����ǰ�Ĺ۲췶Χ
	inline const b2Vec2& GetBodyPos();//��ȡ����ǰ��ҵĵ�λ 

	inline bool EnterView(ViewRange* actor);//ĳһ����ɫ������Ұ 
	inline void LeaveView(ActorID actorID); //ĳһ����ɫ�뿪����ͼ 
	inline bool EnterBeObseverView(ViewRange* actor);//ĳһ����ɫ���뵽�˱��۲����ͼ

	inline bool InquiryBeObserver(ViewRange* actor); //�����ƶ�AABB�ǰ����Լ�
	inline void ClearObserverTable();//����ǰ�۲쵽�����н�ɫ

	inline int JudgeActorViewAction();//�����Ӳ�,���ڶ��ٲŻ��ؼ��� 
	inline void MoveSelf(); //֪ͨÿ���۲쵽�Լ�����,��ǰ�Լ��ƶ���  
	inline void RecalcBodyPosition();//���»�ȡ����ɫ��һ��δˢ�µ�λ��

	inline const b2AABB& GetBodyAABB(PointType type);//��ȡ����ǰ��ɫ�ĸ���AABB  
	inline const std::list<b2AABB*>& GetSplitAABB(PointType type);//��ȡ����ǰ�Ĳü�AABB 
	inline void RecalcAABBRange(PointType type);//���¼����ӿ� 
	inline void CalcSplitAABB(PointType type);//����ָ����(AABB�ᾭ����,��ͼ��������)
	inline void RecalcBodyAABB(PointType type);//���¼����ӿ�
	inline void RecycleAABBPoint(PointType type);//���յ�λ 
	~ViewRange();  
private:
	BodyData* m_Actor;//��ǰ�Ľ�ɫ 
	std::unordered_map<ActorID, ViewRange*> m_ObserverMap;//���Կ����Ľ�ɫ�б�  
	std::unordered_map<ActorID, ViewRange*> m_BeObserverMap;//���ڼ�¼ÿ����ɫ��˭�۲쵽 
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ  
	b2Vec2 m_BodyPos;//��һ�εĽ�ɫλ��
	b2AABB m_BodyAABB[PointType::MAX_POINT_TYPE];//��¼��ɫ��һ�����ʱ��AABB     
	std::list<b2AABB*> m_SplitAABBList[PointType::MAX_POINT_TYPE];//���յ�,�ָ���AABB
	AutomaticGenerator<b2AABB>* m_AABBAllocObj;//����һ�·���AABB�Ķ���
}; 

class AxisDistanceManager
{
public:  
	inline void AddtionCalcBody(ActorID id);
	inline bool RegisterBody(ActorID id);
	//���¼����ɫ����
	void RecalcActorDistance();
	inline void RemoveDistanceAABB(ViewRange* actoViewRange, PointType type);
	//��ʹ�ñ�����֮ǰӦȷ��,��ǰ���б���,���޶�Ӧ��ɫ
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange);
	inline void RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange);
	inline void ABBBRnageRecycle(float pos, bool isX);
	void MoveActor();
	//Ѱ��ĳһ�����ĳһ��Χ�ڵ����е�Ԫ��Ϣ
	bool InquiryAxisPoints(std::unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset = 0, float rightOffset = 0);//ƫ�Ƶ�Ԫ����ǰҪ��ѯ����Ϣ(����ȷ����ǰ����ѯ������ڱ���)
	//�ؼ���
	inline void CalcViewObj();
	void UnregisterBody(ActorID id);
private:
	//�ڴ�������
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //�Զ���B2AABB�������
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//�Զ���������ͼ�������Ķ��� 
	//��ǰ���еľ������
	ViewRangeRecordMap m_AxisBodyMap[AxisType::MAX_AXIS];//������X�����к�Y������ 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//��ɫ��Ӧ�ķ�Χ��Ϣ 
	std::unordered_set<ActorID> m_DelayCalcMoveList;//��ɫ��Ӧ�ķ�Χ��Ϣ
	AxisDistanceManager() {}
}; 
