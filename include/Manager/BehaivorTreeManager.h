#pragma once 
#include "../Library/BehaviorTree/behavior_tree.h"
#include "../Library/BehaviorTree/bt_factory.h"
#include "../Library/BehaviorTree/action_node.h" 
#include "Manager/Base/BodyData.h" 
#include "BodyManager.h"
#include "TemplateBoard.h"
#include <map>   
using namespace std;
using namespace BT;
//��Ϊ�� �������ڿ��ƽ�ɫ���ƶ�����
//ÿ����ɫ�� ������Ϊ
//�ƶ�,����,����,����,��Ĭ,����,����,����,����,��,����,
//���Ǻ͹������Ϊ���ǲ�һ����
enum BTreeType {
	BT_ACTOR_TYPE = 0,//�������
	BT_MONSTER_TYPE = 1,//��������
	BT_BULLET_TYPE = 2,//�ӵ�����
};
struct BTreeTypePlane{
	BTreeType m_Type;
	Tree* m_BTree;
};
class BehaivorTreeManager
{ 
public:  
	map<BTreeType, BTreeTypePlane*> m_BTreeMap;//��Ϊ��ӵ�ж���
	map<BTreeType, AutomaticGenerator<BTreeTypePlane>*> m_BTIdleTypeMap;//��Ϊ�����ж���,���ڿ��ٴӶ�����ȡ��Ϊ��
	map<BTreeType, BehaviorTreeFactory*> m_BTFactory;//��Ϊ���Ĺ������� 
public:
	bool RegisterActor(ActorID id, BTreeType type) {
		if (m_BTreeMap.count(id))
			return false;
		if (!m_BTIdleTypeMap.count(type)) {
			m_BTIdleTypeMap[type] = new AutomaticGenerator<BTreeTypePlane>();
		}
		AutomaticGenerator<BTreeTypePlane>* IdleTypeobj = m_BTIdleTypeMap[type];
		m_BTreeMap[id] = IdleTypeobj->RequireObj();//���һ����Ϊ��
		return true;
	}
	bool UnRegisterActor(ActorID id) {
		if (!m_BTreeMap.count(id))
			return false; 
		BTreeTypePlane* plane = m_BTreeMap[id];
		AutomaticGenerator<BTreeTypePlane>* IdleTypeobj = m_BTIdleTypeMap[plane->m_Type];
		IdleTypeobj->BackObj(plane);
		m_BTreeMap.erase(id)
		return true;;
	}
	bool Update()
	{
		for (auto item = m_BTreeMap.begin();item != m_BTreeMap.end();item++) {
			item->second->m_BTree->tickRoot();
		} 
public:
	BehaivorTreeManager::Instance();
}; 
 
inline NavmeshManager& NavmeshManager::Instance() {
	static NavmeshManager Instance;
	return Instance;
}
 