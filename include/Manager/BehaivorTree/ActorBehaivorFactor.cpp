#pragma once 
#include "../Library/BehaviorTree/behavior_tree.h"
#include "../Library/BehaviorTree/bt_factory.h"
#include "../Library/BehaviorTree/action_node.h" 
#include "Manager/Base/BodyData.h" 
#include "BodyManager.h" 
#include <map>   
using namespace std;
class ActorBehaivorFactor
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



	local BehaivorBase = class("BehaivorBase")
		local CoroActionNode = require("ActorBehaivorTree.CoroActionNode")
		function BehaivorBase : ctor(xml)
		self : __InitData(xml)
		end
		--用于数据的初始化
		function BehaivorBase : __InitData(xml)
		self.BehaviorFactor = sol.BTFactory.new() --创建一个工厂
		self : BehaviorInit() --初始化行为树
		self.BehaviorFactor : registerBehaviorTreeFromText(xml)
		end

		--初始化内部函数
		function BehaivorBase : BehaviorInit()
		end

		function BehaivorBase : CreateTree(param)
		local tree = self.BehaviorFactor : createTree()
		tree : InitLuaNodeParam(param)
		return tree
		end
		return BehaivorBase