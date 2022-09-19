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
//行为树 可以用于控制角色的移动数据
//每个角色有 多种行为
//移动,禁锢,奔跑,空闲,沉默,减速,冰冻,隐身,闪现,狂暴,死亡,
//主角和怪物的行为树是不一样的
enum BTreeType {
	BT_ACTOR_TYPE = 0,//玩家类型
	BT_MONSTER_TYPE = 1,//怪物类型
	BT_BULLET_TYPE = 2,//子弹类型
};
struct BTreeTypePlane{
	BTreeType m_Type;
	Tree* m_BTree;
};
class BehaivorTreeManager
{ 
public:  
	map<BTreeType, BTreeTypePlane*> m_BTreeMap;//行为树拥有队列
	map<BTreeType, AutomaticGenerator<BTreeTypePlane>*> m_BTIdleTypeMap;//行为树空闲队列,用于快速从队列中取行为树
	map<BTreeType, BehaviorTreeFactory*> m_BTFactory;//行为树的工厂方法 
public:
	bool RegisterActor(ActorID id, BTreeType type) {
		if (m_BTreeMap.count(id))
			return false;
		if (!m_BTIdleTypeMap.count(type)) {
			m_BTIdleTypeMap[type] = new AutomaticGenerator<BTreeTypePlane>();
		}
		AutomaticGenerator<BTreeTypePlane>* IdleTypeobj = m_BTIdleTypeMap[type];
		m_BTreeMap[id] = IdleTypeobj->RequireObj();//添加一个行为树
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
 