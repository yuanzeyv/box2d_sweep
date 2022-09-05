#pragma once
#include "Manager/Base/BodyData.h"   
#include "Manager/BodyManager.h"  
#include "Manager/AxisDistanceManager.h"
#include <map> 
#include <vector>   
using namespace std;   

ViewRange::ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range = b2Vec2(0.5, 0.5)) :m_ObserverRange(range), m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), m_AABBAllocObj(allocObj)
{
} 

const b2AABB& ViewRange::GetBodyAABB(PointType type)//获取到当前角色的刚体AABB 
{
	return m_BodyAABB[type];
}

const list<b2AABB*>& ViewRange::GetSplitAABB(PointType type)//获取到当前的裁剪AABB
{
	return m_SplitAABBList[type];
}

void ViewRange::RecalcAABBRange(PointType type)//重新计算视口
{
	if (type == PointType::VIEW_TYPE) {
		m_BodyPos = m_Actor->GetBody()->GetPosition();
		m_BodyAABB[type].lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);
		m_BodyAABB[type].upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
	}
	else {
		const b2AABB& bodyAABB = m_Actor->GetAABB();
		m_BodyAABB[type].lowerBound.Set(bodyAABB.lowerBound);
		m_BodyAABB[type].upperBound.Set(bodyAABB.upperBound);
	}
}

void ViewRange::RecalcBodyPosition()
{
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}

void ViewRange::CalcSplitAABB(PointType type)//计算分割刚体(AABB会经常算,视图不经常算)
{   //首先获取到当前刚体的AABB的长度
	float length = m_BodyAABB[type].upperBound.x - m_BodyAABB[type].lowerBound.x;
	int count = ceil(length / MAX_OVERFLOW_RANGE);//计算需要分配多少个  108
	int splitCount = count - m_SplitAABBList[type].size();//获取到当前的分割个数
	if (splitCount < 0) {
		for (int i = 0; i > splitCount; i--) {
			auto obj = m_SplitAABBList[type].begin();
			m_AABBAllocObj->BackObj(*obj);
			m_SplitAABBList[type].erase(obj);
		}
	}
	else if (splitCount > 0) {
		for (int i = 0; i > splitCount; i--) {
			auto obj = m_SplitAABBList[type].begin();
			m_SplitAABBList[type].push_back(m_AABBAllocObj->RequireObj());
		}
	}
	float calcX = m_BodyAABB[type].lowerBound.x;//最开始的位置
	for (auto item = m_SplitAABBList[type].begin(); item != m_SplitAABBList[type].end(); item++)
	{
		b2AABB* aabb = *item;
		aabb->lowerBound.Set(calcX, m_BodyAABB[type].lowerBound.y);
		if (calcX + MAX_OVERFLOW_RANGE < m_BodyAABB[type].upperBound.x)
			aabb->upperBound.Set(calcX + MAX_OVERFLOW_RANGE, m_BodyAABB[type].upperBound.y);
		else
			aabb->upperBound.Set(m_BodyAABB[type].upperBound.y, m_BodyAABB[type].upperBound.y);
	}
}

void ViewRange::RecalcBodyAABB(PointType type)//重新计算视口
{
	RecalcAABBRange(type);//计算基础的AABB
	CalcSplitAABB(type);//计算切割后的AABB
}

void ViewRange::ClearObserverTable() //清理当前观察到的所有角色
{
	m_ObserverMap.clear();
}

const b2Vec2& ViewRange::GetViewRange() //获取到当前的视图范围
{
	return m_ObserverRange;
}

int ViewRange::JudgeActorViewAction()//计算视差,大于多少才会重计算
{
	b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();//获取到当前的位置 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float xPercent = xMinus / (m_ObserverRange.x * 2);
	float yPercent = yMinus / (m_ObserverRange.y * 2);
	float finalPercent = std::max(xPercent, yPercent);
	if (finalPercent <= 3)//视差任意轴移动距离小于百分之3,不做计算 
		return 0;
	return 1;
}

BodyData* ViewRange::GetActor() {//获取到当前的角色
	return m_Actor;
}

bool ViewRange::EnterView(ViewRange* actor)//某一个角色进入视野
{
	m_ObserverMap[actor->GetActor()->ID()] = actor;
	actor->EnterBeObseverView(this);//敌人观察到了自己
}

bool ViewRange::EnterBeObseverView(ViewRange* actor)//某一个角色进入到了被观察的视图
{
	m_BeObserverMap[actor->GetActor()->ID()] = actor;
}

void ViewRange::LeaveView(ActorID actorID) //某一个角色离开了视图
{
	m_ObserverMap.erase(actorID);
}

const b2Vec2& ViewRange::GetBodyPos()//获取到当前玩家的点位
{
	return m_BodyPos;
}

void ViewRange::RecycleAABBPoint(PointType type)//回收点位
{
	for (auto item = m_SplitAABBList->begin(); item != m_SplitAABBList->end();) {
		m_AABBAllocObj->BackObj(*item);//首先返回当前的信息
		m_SplitAABBList->erase(item++);//删除当前这个
	}
}
ViewRange::~ViewRange()
{
	RecycleAABBPoint(PointType::BODY_TYPE);
	RecycleAABBPoint(PointType::VIEW_TYPE);
}
//自己移动的话
void ViewRange::MoveSelf() {//通知每个观察到自己的人,当前自己移动了
	for (auto item = m_BeObserverMap.begin(); item != m_BeObserverMap.end(); ) {
		if (!item->second->InquiryBeObserver(this))//如果自己当前还在对方的视野下
			m_BeObserverMap.erase(item++);
		else
			item++;
	}
}

bool ViewRange::InquiryBeObserver(ViewRange* actor) {//移动的角色询问当前是否还在监听范围内
	bool isObserver = this->GetBodyAABB(PointType::VIEW_TYPE).Contains(actor->GetBodyAABB(PointType::BODY_TYPE));//判断两个AABB是否重叠
	if (!isObserver && m_ObserverMap.count(actor->m_Actor->ID()))//如果当前看得观察到了这个刚体,但现在看不见了
		m_ObserverMap.erase(actor->m_Actor->ID());
	return isObserver;//删除自己
}

  
AxisDistanceManager::AxisDistanceManager() {
}
inline void AxisDistanceManager::AddtionCalcBody(ActorID id) {
	m_DelayCalcMoveList.insert(id);//这个角色将再下一帧被重计算
}
inline bool AxisDistanceManager::RegisterBody(ActorID id) {
	if (m_ViewObjMap.count(id)) return false;//对象已经注册 
	BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//通过身体控制单元获取到对应的身体 
	m_ViewObjMap[id] = new ViewRange(bodyData, &m_AABBIdleGenerate);//注册进入即可
	AddtionCalcBody(id);//在下一帧重新计算角色信息
	return true;
}
//重新计算角色距离
void AxisDistanceManager::RecalcActorDistance() {
	//整体计算一下当前的移动距离
	MoveActor();//更新所有角色的视距,包括碰撞AABB 
	CalcViewObj();//计算角色可以观察到的视图对象 
}
inline void AxisDistanceManager::RemoveDistanceAABB(ViewRange* actoViewRange, PointType type) {
	const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//获取到当前裁切后的AABB
	int index = 1;
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (auto item = splitAABBList.begin(); item != splitAABBList.end(); item++, index++) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//获取到当前的区域ID
		RemoveDistancePoint(actorID, type, *(*item));//删除原有的物理碰撞盒子
	}
}
inline void AxisDistanceManager::AdditionDistanceAABB(ViewRange* actoViewRange, PointType type) {
	const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//获取到当前裁切后的AABB
	int index = 1;
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (auto item = splitAABBList.begin(); item != splitAABBList.end(); item++, index++) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//获取到当前的区域ID
		AdditionDistancePoint(actorID, type, *(*item));//删除原有的物理碰撞盒子
	}
}
//在使用本函数之前应确保,当前的列表中,再无对应角色
inline void AxisDistanceManager::AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //生成最小ID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //生成最大ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];
	//判断有无,申请
	if (!xRangeRecordMap.count(viewRange.lowerBound.x)) {//不存在当前坐标点的话  
		ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
		xRangeRecordMap[viewRange.lowerBound.x] = typeSet;
	}
	if (!xRangeRecordMap.count(viewRange.upperBound.x)) {//不存在当前坐标点的话  
		ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
		xRangeRecordMap[viewRange.upperBound.x] = typeSet;
	}
	if (!yRangeRecordMap.count(viewRange.lowerBound.y)) {//不存在当前坐标点的话  
		ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
		yRangeRecordMap[viewRange.lowerBound.y] = typeSet;
	}
	if (!yRangeRecordMap.count(viewRange.upperBound.y)) {//不存在当前坐标点的话  
		ViewRangeTypeSet* typeSet = m_ViewCellIdleGenerate.RequireObj();
		yRangeRecordMap[viewRange.upperBound.y] = typeSet;
	}
	(*xRangeRecordMap[viewRange.lowerBound.x])[addType].insert(actorID);
	(*xRangeRecordMap[viewRange.upperBound.x])[addType].insert(actorID);
	(*yRangeRecordMap[viewRange.lowerBound.y])[addType].insert(actorID);
	(*yRangeRecordMap[viewRange.upperBound.y])[addType].insert(actorID);
}
inline void AxisDistanceManager::RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange)
{
	ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //生成最小ID
	ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //生成最大ID 
	ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
	ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];
	if (xRangeRecordMap.count(viewRange.lowerBound.x))//删除这个ID对应的X坐标点
		if ((*xRangeRecordMap[viewRange.lowerBound.x])[type].count(minActorID))
			(*xRangeRecordMap[viewRange.lowerBound.x])[type].erase(minActorID);
	if (xRangeRecordMap.count(viewRange.upperBound.x))//存在当前坐标点的话 
		if ((*xRangeRecordMap[viewRange.upperBound.x])[type].count(maxActorID))
			(*xRangeRecordMap[viewRange.upperBound.x])[type].erase(maxActorID);
	if (yRangeRecordMap.count(viewRange.lowerBound.y))//存在当前坐标点的话 
		if ((*yRangeRecordMap[viewRange.lowerBound.y])[type].count(minActorID))
			(*yRangeRecordMap[viewRange.lowerBound.y])[type].erase(minActorID);
	if (yRangeRecordMap.count(viewRange.upperBound.y))//存在当前坐标点的话 
		if ((*yRangeRecordMap[viewRange.upperBound.y])[type].count(maxActorID))
			(*yRangeRecordMap[viewRange.upperBound.y])[type].erase(maxActorID);
	//判断是否使用结束了
	ABBBRnageRecycle(viewRange.lowerBound.x, true);
	ABBBRnageRecycle(viewRange.upperBound.x, true);
	ABBBRnageRecycle(viewRange.lowerBound.y, false);
	ABBBRnageRecycle(viewRange.upperBound.y, false);
}
inline void AxisDistanceManager::ABBBRnageRecycle(float pos, bool isX)
{
	ViewRangeRecordMap& tempMap = isX ? m_AxisBodyMap[AxisType::X_AXIS] : m_AxisBodyMap[AxisType::Y_AXIS];
	if (!tempMap.count(pos))
		return;
	ViewRangeTypeSet* rangeTypeSet = tempMap[pos];
	bool isEmpty = true;
	for (int type = 0; type < MAX_POINT_TYPE; type++) {
		if (!(*rangeTypeSet)[(PointType)type].empty()) {
			isEmpty = false;
			break;
		}
	}
	if (!isEmpty)
		return;
	m_ViewCellIdleGenerate.BackObj(rangeTypeSet);//回收它
	tempMap.erase(pos);
}
void AxisDistanceManager::MoveActor()
{
	//移动角色,如果角色符合移动的条件,则进入下一步视距范围的判断
	ViewRange* actoViewange = NULL;
	bool OnlyCalculate = false;
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//首先计算当前的操作 
		ActorID actorID = *item;
		OnlyCalculate = true;
		if (!m_ViewObjMap.count(actorID))//对象不存在,直接删除
			goto erase;
		actoViewange = m_ViewObjMap[actorID];
		if (actoViewange->JudgeActorViewAction() == 1) {//移动距离过浅
			goto calc_erase;//计算后删除
		}
		OnlyCalculate = true;
	calc_erase:
		//对当前角色进行位移,移动视野  移动碰撞盒
		RemoveDistanceAABB(actoViewange, PointType::BODY_TYPE);//删除body
		RemoveDistanceAABB(actoViewange, PointType::VIEW_TYPE);//删除view 
		actoViewange->RecalcBodyAABB(PointType::BODY_TYPE);//重新计算当前的碰撞盒子
		actoViewange->RecalcBodyAABB(PointType::VIEW_TYPE);//重新计算当前的视图盒子 
		actoViewange->RecalcBodyPosition();//重计算角色的位置
		AdditionDistanceAABB(actoViewange, PointType::BODY_TYPE);//将现有的添加上去
		AdditionDistanceAABB(actoViewange, PointType::VIEW_TYPE);//将现有的添加上去 
		item = item++;
		if (OnlyCalculate)
			continue;
	erase:
		m_DelayCalcMoveList.erase(actorID);
	}
}
//寻找某一个轴的某一范围内的所有单元信息
bool AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset = 0, float rightOffset = 0)//偏移单元代表当前要查询的息(必须确保当前被查询点存在于表内)
{
	if (0 == m_AxisBodyMap[axis].count(findKey))//对象不存在,直接删除 
		return false;
	auto axisMap = m_AxisBodyMap[axis];
	auto beginIterator = axisMap.find(findKey);//也就是必定存在
	std::map<float, ViewRangeTypeSet*>::reverse_iterator rBeginITerator(beginIterator);
	float rightDistance = inquiryDistance + rightOffset;
	//首先循环遍历正向
	for (auto item = beginIterator; item != axisMap.end() && item->first <= rightDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
	//首先获取到当前寻找到的位置
	float leftDistance = inquiryDistance - leftOffset;
	if (leftOffset == 0) {
		goto out;
	}
	rBeginITerator++;//向前移动,避免包含自己
	for (auto item = rBeginITerator; item != axisMap.rend() && item->first >= leftDistance; item++) {
		auto typeMap = (*item->second)[type];
		for (auto distanceItem = typeMap.begin(); distanceItem != typeMap.end(); distanceItem++)
			outData.insert(GET_ACTOR_ID(*distanceItem));
	}
out:
	return true;
}
//重计算
inline void AxisDistanceManager::CalcViewObj()
{
	//2:通知当前观察到该角色的玩家,角色发生了移动,然后进行修改
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//首先计算当前的操作 
		ActorID actorID = *item;
		if (!m_ViewObjMap.count(actorID))//对象不存在,直接删除 
			continue;
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->MoveSelf();//代表自己移动了
	}


	//移动角色,分为三步
	//1:首   先删除当前角色所能看到的所有玩家
	//1-1:重新计算,视图范围内,所看到的玩家数目
	for (auto item = m_DelayCalcMoveList.begin(); item != m_DelayCalcMoveList.end();) {
		//首先计算当前的操作 
		ActorID actorID = *item;
		if (!m_ViewObjMap.count(actorID))//对象不存在,直接删除 
			continue;
		ViewRange* actoViewange = m_ViewObjMap[actorID];
		actoViewange->ClearObserverTable();//清除,然后重新计算
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetBodyAABB(PointType::VIEW_TYPE);
		InquiryAxisPoints(XSet, viewAABB.lowerBound.x, PointType::BODY_TYPE, AxisType::X_AXIS, viewAABB.upperBound.x - viewAABB.lowerBound.x, 0,MAX_OVERFLOW_RANGE);//获取到X轴的列表
		InquiryAxisPoints(XSet, viewAABB.lowerBound.y, PointType::BODY_TYPE, AxisType::Y_AXIS, viewAABB.upperBound.y - viewAABB.lowerBound.y, 0, MAX_OVERFLOW_RANGE);//获取到X轴的列表 
		//所有关联的角色统统计算一下距离

		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem];
			//计算AABB
			if (obseverItem->GetBodyAABB(PointType::BODY_TYPE).Contains(actoViewange->GetBodyAABB(PointType::VIEW_TYPE)))
			{
				actoViewange->EnterView(obseverItem);
			}
		}
	}
}
void AxisDistanceManager::UnregisterBody(ActorID id)
{
	if (m_ViewObjMap.count(id))
		return; //已经注册过了  
	ViewRange* rangeObj = m_ViewObjMap[id];
	RemoveDistanceAABB(rangeObj, PointType::BODY_TYPE);
	RemoveDistanceAABB(rangeObj, PointType::VIEW_TYPE);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}

