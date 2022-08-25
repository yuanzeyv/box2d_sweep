#pragma once
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h" 
#include "Manager/BodyManager.h"
#include "Manager/DistanceManager.h"
#include <map> 
#include <vector>   
using namespace std;  
//注册一个bodyData
bool DistanceManager::RegisterBody(BodyData* bodyData) { 
	DistanceObjMap[bodyData->ID()] = bodyData;//用于存储记录 道具信息
	BodyType type = bodyData->Type();
	if (type == BodyType::MonsterBody) {//如果类型为 怪物类型单位话
		__RegisterBody(bodyData, ViewType::Monster_Hero, BodyType::PlayerBody,ViewType::Hero_Monster); 
	}
	else if (type == BodyType::PlayerBody)
	{
		__RegisterBody(bodyData, ViewType::Hero_Monster, BodyType::MonsterBody,ViewType::Monster_Hero);
		__RegisterBody(bodyData, ViewType::Hero_Hero, BodyType::PlayerBody, ViewType::Null);
		__RegisterBody(bodyData, ViewType::Hero_Static, BodyType::StaticBody, ViewType::Static_Hero);
		__RegisterBody(bodyData, ViewType::Hero_Bullet, BodyType::BulletBody, ViewType::Bullet_Hero);
	}
	else if (type == BodyType::StaticBody)
		__RegisterBody(bodyData, ViewType::Static_Hero, BodyType::PlayerBody, ViewType::Hero_Static);
	else if (type == BodyType::BulletBody) 
		__RegisterBody(bodyData, ViewType::Bullet_Hero , BodyType::PlayerBody, ViewType::Hero_Bullet); 
	return true;
}
void DistanceManager::UnregisterBody(ActorID id)
{
	UnregisterBody(DistanceObjMap[id]);//开始反注册这个距离数据
}

void DistanceManager::UnregisterBody(BodyData* body)
{
	if (body->Type() == BodyType::MonsterBody) {
		__UnregisterBody(body, ViewType::Monster_Hero,ViewType::Hero_Monster);
	}
	else if (body->Type() == BodyType::PlayerBody)
	{
		__UnregisterBody(body, ViewType::Hero_Monster, ViewType::Monster_Hero);
		__UnregisterBody(body, ViewType::Hero_Hero, ViewType::Null);
		__UnregisterBody(body, ViewType::Hero_Static, ViewType::Static_Hero);
		__UnregisterBody(body, ViewType::Hero_Bullet, ViewType::Bullet_Hero);
	}
	else if (body->Type() == BodyType::StaticBody)
	{
		__UnregisterBody(body, ViewType::Static_Hero, ViewType::Hero_Static);
	}
	else if (body->Type() == BodyType::BulletBody)
	{ 
		__UnregisterBody(body, ViewType::Bullet_Hero, ViewType::Hero_Bullet);
	} 
	IgnoreCalcMap.erase(body->ID());
	DistanceObjMap.erase(body->ID());
	DistanceCalcRemove(body);
}

//获取到视野范围内 最近的一个BodyData
b2Body* DistanceManager::MinumumDistanceBody(ActorID checkID, ViewType checkType)
{ 
	BodyData* bodyData = DistanceObjMap[checkID];
	auto visibleList = DistanceMap[checkType][checkID][ViewStatus::Visble];
	float distance = 9999;
	b2Body* retBody = NULL;
	b2Vec2 bodyPos = bodyData->GetBody()->GetPosition();
	for (auto begin = visibleList.begin(); begin != visibleList.end(); begin++) {
		b2Body* tempBody = begin->second->GetBody();
		b2Vec2 dir = bodyPos - tempBody->GetPosition();//首先通过向量差 获取到最近的一个点 
		if (dir.Length() > distance && checkID != begin->first) {
			distance = dir.Length();
			retBody = tempBody;
		}
	}
	return retBody;
}
void DistanceManager::RecountDistance(BodyData * body) {
	if (body->Type() == BodyType::MonsterBody) { 
		__RecountDistance(body, ViewType::Monster_Hero, ViewType::Hero_Monster);
	} else if (body->Type() == BodyType::PlayerBody) {
		__RecountDistance(body, ViewType::Hero_Monster, ViewType::Monster_Hero);
		__RecountDistance(body, ViewType::Hero_Hero, ViewType::Null);
		__RecountDistance(body, ViewType::Hero_Static, ViewType::Null);
		__RecountDistance(body, ViewType::Hero_Bullet, ViewType::Bullet_Hero);
	}else if (body->Type() == BodyType::BulletBody) {
		__RecountDistance(body, ViewType::Bullet_Hero, ViewType::Hero_Bullet);
	}
}

//重新计算距离
void DistanceManager::__RecountDistance(BodyData* body, ViewType viewType, ViewType addType)
{
	auto& inviewMap = DistanceMap[viewType][body->ID()][ViewStatus::Invisble];//为了方便计算 获取到不可视表
	auto& viewMap = DistanceMap[viewType][body->ID()][ViewStatus::Visble];//为了方便计算 获取到可视表
	map<ActorID, BodyData*> tempInviewMap;//临时存储，以防不好操作Map结构
	const b2AABB& BodyAABB = body->GetViewAABB();//获取到可视范围
	for (auto begin = viewMap.begin(); begin != viewMap.end(); ) {//开始计算看得见的表 
		if (!b2TestOverlap(BodyAABB, begin->second->GetAABB())) { //如果当前变的不可视了
			tempInviewMap.insert(*begin);//插入到临时 
			viewMap.erase(begin++);//可视Map删除本对象
		}
		else {
			begin++;
		}
	}
	for (auto begin = inviewMap.begin(); begin != inviewMap.end(); ) {//再次遍历不可见  
		if (b2TestOverlap(BodyAABB, begin->second->GetAABB())) { //如果当前变更为可见了 
			viewMap.insert(*begin);//插入到可见
			inviewMap.erase(begin++);//从不可见列表中删除
		}
		else {
			begin++;
		}
	}
	inviewMap.insert(tempInviewMap.begin(), tempInviewMap.end());//不可视临时Map添加
	if (addType == ViewType::Null)//不执行之后的操作
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::Visble];//获取到被操作对象的所有数据信息
		auto& typeMap = DistanceMap[addType];//获取到 关联对象的Map
		for (auto item = typeMap.begin(); item != typeMap.end(); item++) {//对相关联的表进行更新
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::Visble : ViewStatus::Invisble;//判断其是否在可见列表中，得出的结果是当前所在的列表
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::Visble ? ViewStatus::Invisble : ViewStatus::Visble;//判断其是否在可见列表中，得出的结果是当前所在的列表
			auto relevanceVisibleMap = typeMap[item->first][isMainVisble];//获取到对应关联列表列表
			ViewStatus isRelevanceVisible = relevanceVisibleMap.find(body->ID()) != relevanceVisibleMap.end() ? isMainVisble : isInverterVisble;//查找列表是否有这个单元，如果没有，
			if (isMainVisble != isRelevanceVisible) {
				typeMap[item->first][isInverterVisble].erase(body->ID());
				typeMap[item->first][isMainVisble][body->ID()] = body;
			}
		}
	}
con:
	return;
}

//添加一个单元 将所有的 listType 下的单元加入到 viewType 下，并将BodyData单元加入到 addType下
void DistanceManager::__RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType)
{
	const b2AABB& BodyAABB = body->GetViewAABB(); //首先获取到身体的视野范围
	DistanceMap[viewType][body->ID()] = unordered_map< ViewStatus, unordered_map<ActorID, BodyData*>>();//对其赋值
	auto BodyList = BodyManager::Instance().GetBodyList(listType);//获取将关联的玩家列表 
	for (auto begin = BodyList.begin(); begin != BodyList.end();begin++){//开始遍历玩家列表
		ActorID ID = begin->second->ID();
		BodyData* calcBody = DistanceObjMap[ID];
		ViewStatus viewStatus = b2TestOverlap(BodyAABB, calcBody->GetAABB()) ? ViewStatus::Visble : ViewStatus::Invisble;//玩家是否出现在怪物视野范围内
		DistanceMap[viewType][body->ID()][viewStatus][begin->first] = calcBody; //将玩家添加到对应视野范围
	} 
	if (addType == ViewType::Null)//是否与其他表联动
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::Visble];//获取到被操作对象的所有数据信息
		auto& typeMap = DistanceMap[addType];//获取到 关联对象的Map
		for (auto item = typeMap.begin();item != typeMap.end();item++) {//对相关联的表进行更新
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::Visble : ViewStatus::Invisble;//判断其是否在可见列表中，得出的结果是当前所在的列表
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::Visble ? ViewStatus::Invisble : ViewStatus::Visble;//判断其是否在可见列表中，得出的结果是当前所在的列表
			auto relevanceVisibleMap = typeMap[item->first][isMainVisble];//获取到对应关联列表列表
			ViewStatus isRelevanceVisible = relevanceVisibleMap.find(body->ID()) != relevanceVisibleMap.end() ? isMainVisble : isInverterVisble;//查找列表是否有这个单元，如果没有，
			if (isMainVisble != isRelevanceVisible) {
				typeMap[item->first][isInverterVisble].erase(body->ID());
				typeMap[item->first][isMainVisble][body->ID()] = body;
			}
		}
	} 
con:
	return;
}
//反注册一个身体
void DistanceManager::__UnregisterBody(BodyData* body, ViewType viewType, ViewType addType)
{
	auto viewMap = DistanceMap[viewType][body->ID()];
	auto visibleMap = viewMap[ViewStatus::Visble];
	auto invisibleMap = viewMap[ViewStatus::Invisble];
	if (addType != ViewType::Null)//是否与其他表联动 
	{	
		//遍历可以看到的成员
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++) 
			DistanceMap[addType][begin->first][ViewStatus::Visble].erase(body->ID()); 
		//遍历不可以看到的成员
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++) 
			DistanceMap[addType][begin->first][ViewStatus::Invisble].erase(body->ID()); 
	}
	DistanceMap[viewType].erase(body->ID());  
}
void DistanceManager::DistanceCalc() {    
	if (!DelayedCalcMap.size())
		return;
	for (auto item = DelayedCalcMap.begin();item != DelayedCalcMap.end();item++)
	{
		BodyData* body = DistanceObjMap[*item];//通过ID获取到距离数据信息
		body->CalcBodyAABB();//重新计算当期那的AABB
		body->CalcViewBody();//重新计算当前的视图AABB
		if (IgnoreCalcMap.find(body->ID()) != IgnoreCalcMap.end())//如果当前位置发生了变动
			if (b2TestOverlap(body->GetAABB(), IgnoreCalcMap[body->ID()]))//判断当前AABB是 否与 宽松身位有重叠
				continue;//必须移动了半个身位，才能再次参加距离计算  
		IgnoreCalcMap[body->ID()] = body->GetAABB();//重新计算身位 
		RecountDistance(body); 
	} 
	DistanceCalcClear();
}


void DistanceManager::DistanceDump(ActorID id) {
	BodyData* bodyData = DistanceObjMap[id];
	if (bodyData->Type() == BodyType::MonsterBody)
	{
		__Dump(id, ViewType::Monster_Hero);
	}
	else if (bodyData->Type() == BodyType::PlayerBody)
	{
		__Dump(id, ViewType::Hero_Hero);
		__Dump(id, ViewType::Hero_Monster);
	}
}
//打印当前可见对象
void DistanceManager::__Dump(ActorID id , ViewType type)
{
	auto visbleMap = DistanceMap[type][id][ViewStatus::Visble];
	auto invisbleMap = DistanceMap[type][id][ViewStatus::Invisble];
	printf("当前%lld可见的对象 : ", id);
	for (auto begin = visbleMap.begin(); begin != visbleMap.end(); begin++) {//开始计算看得见的表  
		printf("%lld ", begin->first);
	}
	printf("\n\r");
	printf("当前%lld不可见的对象 : ", id);
	for (auto begin = invisbleMap.begin(); begin != invisbleMap.end(); begin++) {//开始计算看得见的表  
		printf("%lld ", begin->first);
	}
	printf("\n\r");
}