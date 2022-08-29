#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <vector>     
//��ǰ��ɫ���Կ���˭
//��ǰ˭���Կ�����ɫ
using std::unordered_map;
using std::map;
using std::unordered_set;
using 
class AxisDistanceManager;
class ViewRange {
public: 
	BodyData*  m_Actor;//��ǰ�Ľ�ɫ
	unordered_map<ActorID, ViewRange*> m_ObserverMap;//���Կ����Ľ�ɫ�б�
	unordered_map<ActorID, ViewRange*> m_BeObserverMap;//��˭�۲쵽
	b2Vec2 m_ObserverRange;//��ǰ��ɫ���Թ۲쵽�ķ�Χ
	b2Vec2 m_BodyPos;//��ɫ��¼�ڰ���λ��,���ڲ�ѯ��ɫ
public:
	inline ViewRange(BodyData* bodyData) 
	{
		Reset();//����һ������
		m_Actor = bodyData;
	}  
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_BodyPos.Set(0, 0);
		m_ObserverMap.clear();
		m_BeObserverMap.clear();
	}
	//һ���˽���Ұ
	bool EnterView(vector<>){

	}
	~ViewRange()
	{ 
	}
};
class AxisDistanceManager
{
private:
	AxisDistanceManager() {}//����һ������������
public: 
	//��ǰ���еľ������
	map<float,unordered_map<ActorID,BodyData*>> XAxisBodyMap;//x����Ľ�ɫ��¼
	map<float,unordered_map<ActorID,BodyData*>> YAxisBodyMap;//y����Ľ�ɫ��¼

	map<ActorID, ViewRange*> ViewRangeMap;//��ɫ��Ӧ��x����y�� 

	inline bool RegisterBody(ActorID id)
	{
		if (ViewRangeMap.count(id)) {
			return false; //�Ѿ�ע�����
		}
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//ͨ��������Ƶ�Ԫ��ȡ����Ӧ������
		ViewRangeMap[id] = new ViewRange(bodyData);
		return true;
	}
	void UnregisterBody(ActorID id)
	{
		if (ViewRangeMap.count(id)) 
			return ; //�Ѿ�ע����� 

		delete ViewRangeMap[id];
		ViewRangeMap.erase(id);
	}

	//������һ����ɫ�˳����й����Ľ�ɫ��Ұ
	void Force

	void UnregisterBody(BodyData* body);
	unordered_map<ActorID, BodyData*>& GetViewBody(ActorID id, ViewType type, ViewStatus view);
	//��ȡ����Ұ��Χ�� �����һ��BodyData 
	b2Body* MinumumDistanceBody(ActorID id, ViewType checkType);
	//֡��ʱ
	void DistanceCalcAdd(b2Body* body);
	void DistanceCalcAdd(BodyData* BodyData);
	void DistanceCalcRemove(b2Body* body);
	void DistanceCalcRemove(BodyData* BodyData);
	void DistanceCalcClear();
	void DistanceCalc(); 


	unordered_map<ViewType, unordered_map<ActorID, unordered_map<ViewStatus, unordered_map<ActorID,BodyData*>>>> DistanceMap;  
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
}; 
