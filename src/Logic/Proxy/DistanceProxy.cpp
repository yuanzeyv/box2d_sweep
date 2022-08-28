#include "Logic/Proxy/DistanceProxy.hpp"
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h"  
#include <map> 
#include <vector>   
using namespace std;
//注册一个bodyData
bool DistanceProxy::RegisterBody(BodyData* bodyData) {
	DistanceObjMap[bodyData->ID()] = bodyData;//用于存储记录 道具信息
	BodyType type = bodyData->Type();
	if (type == BodyType::MONSTER_BODY) {//如果类型为 怪物类型单位话
		__RegisterBody(bodyData, ViewType::MONSTER_HERO, BodyType::PLAYER_BODY, ViewType::HERO_MONSTER);
	}
	else if (type == BodyType::PLAYER_BODY)
	{
		__RegisterBody(bodyData, ViewType::HERO_MONSTER, BodyType::MONSTER_BODY, ViewType::MONSTER_HERO);
		__RegisterBody(bodyData, ViewType::HERO_HERO, BodyType::PLAYER_BODY, ViewType::VIEW_TYPE_NULL);
		__RegisterBody(bodyData, ViewType::HERO_STATIC, BodyType::STATIC_BODY, ViewType::STATIC_HERO);
		__RegisterBody(bodyData, ViewType::HERO_BULLET, BodyType::BULLET_BODY, ViewType::BULLET_HERO);
	}
	else if (type == BodyType::STATIC_BODY)
		__RegisterBody(bodyData, ViewType::STATIC_HERO, BodyType::PLAYER_BODY, ViewType::HERO_STATIC);
	else if (type == BodyType::BULLET_BODY)
		__RegisterBody(bodyData, ViewType::BULLET_HERO, BodyType::PLAYER_BODY, ViewType::HERO_BULLET);
	return true;
}
void DistanceProxy::UnregisterBody(ActorID id)
{
	UnregisterBody(DistanceObjMap[id]);//开始反注册这个距离数据
}

void DistanceProxy::UnregisterBody(BodyData* body)
{
	if (body->Type() == BodyType::MONSTER_BODY) {
		__UnregisterBody(body, ViewType::MONSTER_HERO, ViewType::HERO_MONSTER);
	}
	else if (body->Type() == BodyType::PLAYER_BODY)
	{
		__UnregisterBody(body, ViewType::HERO_MONSTER, ViewType::MONSTER_HERO);
		__UnregisterBody(body, ViewType::HERO_HERO, ViewType::VIEW_TYPE_NULL);
		__UnregisterBody(body, ViewType::HERO_STATIC, ViewType::STATIC_HERO);
		__UnregisterBody(body, ViewType::HERO_BULLET, ViewType::BULLET_HERO);
	}
	else if (body->Type() == BodyType::STATIC_BODY)
	{
		__UnregisterBody(body, ViewType::STATIC_HERO, ViewType::HERO_STATIC);
	}
	else if (body->Type() == BodyType::BULLET_BODY)
	{
		__UnregisterBody(body, ViewType::BULLET_HERO, ViewType::HERO_BULLET);
	}
	IgnoreCalcMap.erase(body->ID());
	DistanceObjMap.erase(body->ID());
	DistanceCalcRemove(body);
}

//获取到视野范围内 最近的一个BodyData
b2Body* DistanceProxy::MinumumDistanceBody(ActorID checkID, ViewType checkType)
{
	BodyData* bodyData = DistanceObjMap[checkID];
	auto visibleList = DistanceMap[checkType][checkID][ViewStatus::VISIBLE];
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
void DistanceProxy::RecountDistance(BodyData* body) {
	if (body->Type() == BodyType::MONSTER_BODY) {
		__RecountDistance(body, ViewType::MONSTER_HERO, ViewType::HERO_MONSTER);
	}
	else if (body->Type() == BodyType::PLAYER_BODY) {
		__RecountDistance(body, ViewType::HERO_MONSTER, ViewType::MONSTER_HERO);
		__RecountDistance(body, ViewType::HERO_HERO, ViewType::VIEW_TYPE_NULL);
		__RecountDistance(body, ViewType::HERO_STATIC, ViewType::VIEW_TYPE_NULL);
		__RecountDistance(body, ViewType::HERO_BULLET, ViewType::BULLET_HERO);
	}
	else if (body->Type() == BodyType::BULLET_BODY) {
		__RecountDistance(body, ViewType::BULLET_HERO, ViewType::HERO_BULLET);
	}
}

//重新计算距离
void DistanceProxy::__RecountDistance(BodyData* body, ViewType viewType, ViewType addType)
{
	auto& inviewMap = DistanceMap[viewType][body->ID()][ViewStatus::INVISIBLE];//为了方便计算 获取到不可视表
	auto& viewMap = DistanceMap[viewType][body->ID()][ViewStatus::VISIBLE];//为了方便计算 获取到可视表
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
	if (addType == ViewType::VIEW_TYPE_NULL)//不执行之后的操作
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::VISIBLE];//获取到被操作对象的所有数据信息
		auto& typeMap = DistanceMap[addType];//获取到 关联对象的Map
		for (auto item = typeMap.begin(); item != typeMap.end(); item++) {//对相关联的表进行更新
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::VISIBLE : ViewStatus::INVISIBLE;//判断其是否在可见列表中，得出的结果是当前所在的列表
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::VISIBLE ? ViewStatus::INVISIBLE : ViewStatus::VISIBLE;//判断其是否在可见列表中，得出的结果是当前所在的列表
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
void DistanceProxy::__RegisterBody(BodyData* body, ViewType viewType, BodyType listType, ViewType addType)
{
	const b2AABB& BodyAABB = body->GetViewAABB(); //首先获取到身体的视野范围
	DistanceMap[viewType][body->ID()] = unordered_map< ViewStatus, unordered_map<ActorID, BodyData*>>();//对其赋值
	auto BodyList = BodyManager::Instance().GetBodyList(listType);//获取将关联的玩家列表 
	for (auto begin = BodyList.begin(); begin != BodyList.end(); begin++) {//开始遍历玩家列表
		ActorID ID = begin->second->ID();
		BodyData* calcBody = DistanceObjMap[ID];
		ViewStatus viewStatus = b2TestOverlap(BodyAABB, calcBody->GetAABB()) ? ViewStatus::VISIBLE : ViewStatus::INVISIBLE;//玩家是否出现在怪物视野范围内
		DistanceMap[viewType][body->ID()][viewStatus][begin->first] = calcBody; //将玩家添加到对应视野范围
	}
	if (addType == ViewType::VIEW_TYPE_NULL)//是否与其他表联动
		goto con;
	{
		auto& visbleMainMap = DistanceMap[viewType][body->ID()][ViewStatus::VISIBLE];//获取到被操作对象的所有数据信息
		auto& typeMap = DistanceMap[addType];//获取到 关联对象的Map
		for (auto item = typeMap.begin(); item != typeMap.end(); item++) {//对相关联的表进行更新
			ViewStatus isMainVisble = visbleMainMap.find(item->first) != visbleMainMap.end() ? ViewStatus::VISIBLE : ViewStatus::INVISIBLE;//判断其是否在可见列表中，得出的结果是当前所在的列表
			ViewStatus isInverterVisble = isMainVisble == ViewStatus::VISIBLE ? ViewStatus::INVISIBLE : ViewStatus::VISIBLE;//判断其是否在可见列表中，得出的结果是当前所在的列表
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
void DistanceProxy::__UnregisterBody(BodyData* body, ViewType viewType, ViewType addType)
{
	auto viewMap = DistanceMap[viewType][body->ID()];
	auto visibleMap = viewMap[ViewStatus::VISIBLE];
	auto invisibleMap = viewMap[ViewStatus::INVISIBLE];
	if (addType != ViewType::VIEW_TYPE_NULL)//是否与其他表联动 
	{
		//遍历可以看到的成员
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++)
			DistanceMap[addType][begin->first][ViewStatus::VISIBLE].erase(body->ID());
		//遍历不可以看到的成员
		for (auto begin = visibleMap.begin(); begin != visibleMap.end(); begin++)
			DistanceMap[addType][begin->first][ViewStatus::INVISIBLE].erase(body->ID());
	}
	DistanceMap[viewType].erase(body->ID());
}
void DistanceProxy::DistanceCalc() {
	if (!DelayedCalcMap.size())
		return;
	for (auto item = DelayedCalcMap.begin(); item != DelayedCalcMap.end(); item++)
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


void DistanceProxy::DistanceDump(ActorID id) {
	BodyData* bodyData = DistanceObjMap[id];
	if (bodyData->Type() == BodyType::MONSTER_BODY)
	{
		__Dump(id, ViewType::MONSTER_HERO);
	}
	else if (bodyData->Type() == BodyType::PLAYER_BODY)
	{
		__Dump(id, ViewType::HERO_HERO);
		__Dump(id, ViewType::HERO_MONSTER);
	}
}
//打印当前可见对象
void DistanceProxy::__Dump(ActorID id, ViewType type)
{
	auto visbleMap = DistanceMap[type][id][ViewStatus::VISIBLE];
	auto invisbleMap = DistanceMap[type][id][ViewStatus::INVISIBLE];
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