#include "LuaBridge/BehaviorTreePort.h" 
#include "Library/BehaviorTree/behavior_tree.h"
#include "Library/BehaviorTree/bt_factory.h"
#include "Library/BehaviorTree/action_node.h"
#include <iostream>
#include <string>
using namespace BT; 
void BehaviorTreePort::InitBehaviorTreeFactory(sol::table solTable)
{
	auto BehaviorTreeFactoryTable = solTable.new_usertype<BehaviorTreeFactory>("BTFactory", sol::constructors<BehaviorTreeFactory()>());
	BehaviorTreeFactoryTable["unregisterBuilder"] = &BehaviorTreeFactory::unregisterBuilder; //��ע��һ��ID  
	BehaviorTreeFactoryTable["RegisterCoroAction"] = &BehaviorTreeFactory::RegisterCoroAction;
	BehaviorTreeFactoryTable["RegisterConditionAction"] = &BehaviorTreeFactory::RegisterLuaCondition;
	BehaviorTreeFactoryTable["registerBehaviorTreeFromFile"] = &BehaviorTreeFactory::registerBehaviorTreeFromFile;
	BehaviorTreeFactoryTable["registerBehaviorTreeFromText"] = &BehaviorTreeFactory::registerBehaviorTreeFromText;
	BehaviorTreeFactoryTable["registeredBehaviorTrees"] = &BehaviorTreeFactory::registeredBehaviorTrees;//ע�����Ϊ����Ϣ 
	BehaviorTreeFactoryTable["builtinNodes"] = &BehaviorTreeFactory::builtinNodes;//��ȡ�����е�����ID  
	BehaviorTreeFactoryTable["manifests"] = &BehaviorTreeFactory::manifests;//��ȡ����ǰ��ʾ�ĸ��� 
	BehaviorTreeFactoryTable["createTreeFromFile"] = &BehaviorTreeFactory::createTreeFromFile;
	BehaviorTreeFactoryTable["createTreeFromText"] = [](BehaviorTreeFactory* self, const std::string& text)->Tree { return self->createTreeFromText(text); };
	BehaviorTreeFactoryTable["createTree"] = [](BehaviorTreeFactory* self)-> Tree { 
		auto obj = self->createTree("MainTree");
		return obj;
	}; 
}
void BehaviorTreePort::InitBTTree(sol::table solTable)
{
	auto BTTreeTable = solTable.new_usertype<Tree>("BTTree", sol::constructors<Tree()>());
	BTTreeTable["nodes"] = &Tree::nodes;//��ע��һ��ID 
	BTTreeTable["blackboard_stack"] = &Tree::blackboard_stack;//��ע��һ��ID 
	BTTreeTable["manifests"] = &Tree::manifests;//��ע��һ��ID   
	BTTreeTable["initialize"] = &Tree::initialize;//��ע��һ��ID   
	BTTreeTable["haltTree"] = &Tree::haltTree;//��ע��һ��ID   
	BTTreeTable["rootNode"] = &Tree::rootNode;//��ע��һ��ID   
	BTTreeTable["tickRoot"] = &Tree::tickRoot;//��ע��һ��ID  
	BTTreeTable["InitLuaNodeParam"] = &Tree::InitLuaNodeParam;//��ʼ��Lua����   
	BTTreeTable["sleep"] = &Tree::sleep;//��ע��һ��ID     
}
void BehaviorTreePort::InitActionNodeBase(sol::table solTable)
{
	auto ActionNodeBaseTable = solTable.new_usertype<ActionNodeBase>("ActionNodeBase");
	ActionNodeBaseTable["halt"] = &ActionNodeBase::halt;
	ActionNodeBaseTable["isHalted"] = &ActionNodeBase::isHalted;
	ActionNodeBaseTable["status"] = &ActionNodeBase::status;
	ActionNodeBaseTable["name"] = &ActionNodeBase::name;
	ActionNodeBaseTable["waitValidStatus"] = &ActionNodeBase::waitValidStatus;
	ActionNodeBaseTable["type"] = &ActionNodeBase::type;
	ActionNodeBaseTable["UID"] = &ActionNodeBase::UID;
	ActionNodeBaseTable["registrationName"] = &ActionNodeBase::registrationName;
	ActionNodeBaseTable["config"] = &ActionNodeBase::config;
	ActionNodeBaseTable["emitStateChanged"] = &ActionNodeBase::emitStateChanged;
	ActionNodeBaseTable["GetInput"] = [](ActionNodeBase* self, const std::string& key)-> sol::lua_value {
		auto oper = self->getInput<std::string>(key);
		if (oper.has_value())
			return oper.value();
		else
			return sol::nil;
	};
	ActionNodeBaseTable["SetOutput"] = [](ActionNodeBase* self, const std::string& key, const std::string& value)->bool {
		return  self->setOutput<std::string>(key, value).has_value();
	}; 
}

void BehaviorTreePort::InitLuaCoroAction(sol::table solTable)
{
	auto LuaCoroActionTable = solTable.new_usertype<LuaCoroAction>("LuaCoroAction");
	LuaCoroActionTable[sol::base_classes] = sol::bases<ActionNodeBase>();
	LuaCoroActionTable["ThreadID"] = &LuaCoroAction::ThreadID;
	
}
void BehaviorTreePort::InitConditionNode(sol::table solTable)
{
	auto ConditionNodeTable = solTable.new_usertype<ConditionNode>("ConditionNode");
	ConditionNodeTable["halt"] = &ConditionNode::halt;
	ConditionNodeTable["isHalted"] = &ConditionNode::isHalted;
	ConditionNodeTable["status"] = &ConditionNode::status;
	ConditionNodeTable["name"] = &ConditionNode::name;
	ConditionNodeTable["waitValidStatus"] = &ConditionNode::waitValidStatus;
	ConditionNodeTable["type"] = &ConditionNode::type;
	ConditionNodeTable["UID"] = &ConditionNode::UID;
	ConditionNodeTable["registrationName"] = &ConditionNode::registrationName;
	ConditionNodeTable["config"] = &ConditionNode::config;
	ConditionNodeTable["emitStateChanged"] = &ConditionNode::emitStateChanged;
	ConditionNodeTable["GetInput"] = [](ConditionNode* self, const std::string& key)-> sol::lua_value {
		auto oper = self->getInput<std::string>(key);
		if (oper.has_value())
			return oper.value();
		else
			return sol::nil;
	};
	ConditionNodeTable["SetOutput"] = [](ConditionNode* self, const std::string& key, const std::string& value)->bool {
		return  self->setOutput<std::string>(key, value).has_value();
	};
}

void BehaviorTreePort::InitLuaConditionNode(sol::table solTable)
{
	auto LuaConditionNodeTable = solTable.new_usertype<LuaConditionNode>("LuaConditionNode");
	LuaConditionNodeTable[sol::base_classes] = sol::bases<ConditionNode>();
}
 
void BehaviorTreePort::InitTable(sol::table solTable) 
{
	InitBehaviorTreeFactory(solTable);
	InitBTTree(solTable);
	InitActionNodeBase(solTable);
	InitLuaCoroAction(solTable);
	InitConditionNode(solTable);
	InitLuaConditionNode(solTable);
}
