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
	BehaviorTreeFactoryTable["unregisterBuilder"] = &BehaviorTreeFactory::unregisterBuilder; //反注册一个ID  
	BehaviorTreeFactoryTable["RegisterCoroAction"] = &BehaviorTreeFactory::RegisterCoroAction;
	BehaviorTreeFactoryTable["RegisterConditionAction"] = &BehaviorTreeFactory::RegisterLuaCondition;
	BehaviorTreeFactoryTable["registerBehaviorTreeFromFile"] = &BehaviorTreeFactory::registerBehaviorTreeFromFile;
	BehaviorTreeFactoryTable["registerBehaviorTreeFromText"] = &BehaviorTreeFactory::registerBehaviorTreeFromText;
	BehaviorTreeFactoryTable["registeredBehaviorTrees"] = &BehaviorTreeFactory::registeredBehaviorTrees;//注册的行为树信息 
	BehaviorTreeFactoryTable["builtinNodes"] = &BehaviorTreeFactory::builtinNodes;//获取到所有的类型ID  
	BehaviorTreeFactoryTable["manifests"] = &BehaviorTreeFactory::manifests;//获取到当前显示的个数 
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
	BTTreeTable["nodes"] = &Tree::nodes;//反注册一个ID 
	BTTreeTable["blackboard_stack"] = &Tree::blackboard_stack;//反注册一个ID 
	BTTreeTable["manifests"] = &Tree::manifests;//反注册一个ID   
	BTTreeTable["initialize"] = &Tree::initialize;//反注册一个ID   
	BTTreeTable["haltTree"] = &Tree::haltTree;//反注册一个ID   
	BTTreeTable["rootNode"] = &Tree::rootNode;//反注册一个ID   
	BTTreeTable["tickRoot"] = &Tree::tickRoot;//反注册一个ID  
	BTTreeTable["InitLuaNodeParam"] = &Tree::InitLuaNodeParam;//初始化Lua参数   
	BTTreeTable["sleep"] = &Tree::sleep;//反注册一个ID     
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
