#pragma once 
#include "BaseProxy.hpp"
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h"
#include <unordered_map>
#include <unordered_set>
#include "Manager/Define.h"
#include <vector>     
using std::unordered_map;
using std::unordered_set;  
class DistanceProxy : public BaseProxy
{
public:
	DistanceProxy():BaseProxy("DistanceProxy") {}//����һ������������
public:
	//��ǰ���еľ������
	unordered_map<ActorID, BodyData*> DistanceObjMap;//��ͼ���� �۲�ID �۲�״̬ ���۲�ID ���۲����   
	unordered_map<ViewType, unordered_map<ActorID, unordered_map<ViewStatus, unordered_map<ActorID, BodyData*>>>> DistanceMap;
	//������λ
	unordered_map<ActorID, b2AABB> IgnoreCalcMap;//������λ
	unordered_set<ActorID> DelayedCalcMap;//��ʱ����  
public:
	bool RegisterBody(BodyData* body);
	void UnregisterBody(ActorID id);
	void UnregisterBody(BodyData* body);
	unordered_map<ActorID, BodyData*>& GetViewBody(ActorID id, ViewType type, ViewStatus view);
	void DistanceDump(ActorID id);
	//��ȡ����Ұ��Χ�� �����һ��BodyData 
	b2Body* MinumumDistanceBody(ActorID id, ViewType checkType);
	//֡��ʱ
	void DistanceCalcAdd(b2Body* body);
	void DistanceCalcAdd(BodyData* BodyData);
	void DistanceCalcRemove(b2Body* body);
	void DistanceCalcRemove(BodyData* BodyData);
	void DistanceCalcClear();
	void DistanceCalc();
	//��ӡ��ǰ�ɼ�����
	void __Dump(ActorID id, ViewType type);
private:
	//���¼���
	inline void RecountDistance(b2Body* body);
	inline void RecountDistance(BodyData* body);
	//���¼������
	void __RecountDistance(BodyData* body, ViewType viewType, ViewType addType);
	//���һ����Ԫ �����е� listType �µĵ�Ԫ���뵽 viewType �£�����BodyData��Ԫ���뵽 addType��
	void __RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType);
	void __UnregisterBody(BodyData* body, ViewType viewType, ViewType addType);
public: 
};
inline void DistanceProxy::DistanceCalcAdd(b2Body* body) {
	if (!DistanceObjMap.count(body->ID()))
		return;
	BodyData* calcBody = DistanceObjMap[body->ID()];
	DistanceCalcAdd(calcBody);
};   
  
inline void DistanceProxy::DistanceCalcAdd(b2Body* body) {
	if (!DistanceObjMap.count(body->ID()))
		return;
	BodyData* calcBody = DistanceObjMap[body->ID()];
	DistanceCalcAdd(calcBody);
}
inline void DistanceProxy::DistanceCalcAdd(BodyData* body) {
	DelayedCalcMap.insert(body->ID());
}

inline void DistanceProxy::DistanceCalcClear() {
	DelayedCalcMap.clear();
}
inline void DistanceProxy::DistanceCalcRemove(b2Body* body) {
	BodyData* calcBody = DistanceObjMap[body->ID()];
	DistanceCalcRemove(calcBody);
}
inline void DistanceProxy::DistanceCalcRemove(BodyData* BodyData) {
	DelayedCalcMap.erase(BodyData->ID());
} 

inline unordered_map<ActorID, BodyData*>& DistanceProxy::GetViewBody(ActorID id, ViewType type, ViewStatus view)
{
	return DistanceMap[type][id][view];
}
inline void DistanceProxy::RecountDistance(b2Body* body)
{
	RecountDistance(DistanceObjMap[body->ID()]);
}
