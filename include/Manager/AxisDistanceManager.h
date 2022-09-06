#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <map>
#include <list>     
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include "Define.h" 
#include "TemplateBoard.h"
#define MAX_OVERFLOW_RANGE (0.5f) //刚体分割距离,也用于检测
//bit 56-64 剩余类型
//bit 48-55 区域ID
//bit 00-47  角色ID
#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ( (type & 1) << 56)) //生成一个设置点位类型的数组 
#define GEN_AABB_ID(actorID,index) (actorID | ((index & 0xffff) << 48 ))//生成一个ID

#define GET_ACTOR_ID(actorID) (actorID & (~0xffff))//获取到当前的角色ID
enum PointPosType {//要不要应该都可以
	POS_BODY_LIMIT_MAX = 0,//点位最大
	POS_BODY_LIMIT_MIN = 1,//点位最小 
};

enum AxisType {
	X_AXIS = 0,//x轴
	Y_AXIS = 1,//y轴
	MAX_AXIS = 2//最大的轴信息
};

enum PointType {
	BODY_TYPE = 0,
	VIEW_TYPE = 1,
	MAX_POINT_TYPE = 2,
}; 
struct ActorComapre {
	bool operator()(ActorID actorIDA, ActorID actorIDB) {
		return actorIDA < actorIDB;
	}
};
class ViewRangeTypeSet {
public:
	std::unordered_set<ActorID, ActorComapre> m_TypeSet[PointType::MAX_POINT_TYPE];
	ViewRangeTypeSet() { 
	}
	inline std::unordered_set<ActorID,ActorComapre>& operator[](PointType type) {
		return m_TypeSet[type];
	}
};
typedef std::map<float, ViewRangeTypeSet*> ViewRangeRecordMap;

   

//当前的视图范围
class ViewRange{
public:
	ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range = b2Vec2(0.5, 0.5));//视图的初始化
	inline BodyData* GetActor(); //获取到当前的角色 
	inline const b2Vec2& GetViewRange(); //获取到当前的观察范围
	inline const b2Vec2& GetBodyPos();//获取到当前玩家的点位 
	inline const std::unordered_map<ActorID, ViewRange*>& GetVisibleMap();//获取到所有可见角色

	inline bool EnterView(ViewRange* actor);//某一个角色进入视野 
	inline void LeaveView(ActorID actorID); //某一个角色离开了视图 
	inline bool EnterBeObseverView(ViewRange* actor);//某一个角色进入到了被观察的视图

	inline bool IsContain(ViewRange* actor); //计算制定AABB是包含自己
	inline void ClearObserverTable();//清理当前观察到的所有角色

	inline int IsRefreshActorView();//计算视差,大于多少才会重计算 
	inline void MoveSelf(); //通知每个观察到自己的人,当前自己移动了  
	inline void RecalcBodyPosition();//重新获取到角色上一次未刷新的位置

	inline const b2AABB& GetBodyAABB(PointType type) const;//获取到当前角色的刚体AABB  
	inline const std::list<b2AABB*>& GetSplitAABB(PointType type);//获取到当前的裁剪AABB 
	inline void RecalcAABBRange(PointType type);//重新计算视口 
	inline void RecalcBodyAABB(PointType type);//重新计算视口
	inline void RecalcRefreshCondtion();//重计算刷新条件  
	inline void CalcSplitAABB(PointType type);//计算分割刚体(AABB会经常算,视图不经常算)
	inline void RecycleAABBPoint(PointType type);//回收点位 
	~ViewRange(); 
private:
	BodyData* m_Actor;//当前的角色 
	b2Vec2 m_BodyPos;//上一次的角色位置

	std::unordered_map<ActorID, ViewRange*> m_ObserverMap;//可以看到的角色列表  
	std::unordered_map<ActorID, ViewRange*> m_BeObserverMap;//用于记录每个角色被谁观察到 
	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围  

	b2AABB m_BodyAABB[PointType::MAX_POINT_TYPE];//记录角色上一次添加时的AABB     
	std::list<b2AABB*> m_SplitAABBList[PointType::MAX_POINT_TYPE];//最终的,分割后的AABB

	b2Vec2 m_OffsetCondtion;//偏移极限

	AutomaticGenerator<b2AABB>* m_AABBAllocObj;//保存一下分配AABB的对象
}; 

class AxisDistanceManager
{
public:
	//注册列表
	inline bool RegisterBody(BodyData* actor);//注册一个对象到距离管理
	inline void UnregisterBody(ActorID id);//反注册
	inline void UnregisterBody(BodyData* actor);//反注册
	//重计算
	void RecalcActorDistance();//重新计算角色距离
	inline void ActorsMove();
	inline void CalcViewObj();//重计算
	//计算区域 
	inline void AddDelayCalcTable(ActorID id);//添加一个刚体计算对象 
	inline void AdditionDistanceAABB(ViewRange* actoViewRange, PointType type);
	inline void RemoveDistanceAABB(ViewRange* actoViewRange, PointType type);
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange);
	inline void RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange); 
	inline void ABBBRnageRecycle(float pos, ViewRangeRecordMap& axisMap);//回收AABB单元
	//获取到单元
	inline ViewRange* GetViewRange(ActorID actorID);

	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager();
	//寻找某一个轴的某一范围内的所有单元信息
	bool InquiryAxisPoints(std::unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset = 0, float rightOffset = 0);//偏移单元代表当前要查询的信息(必须确保当前被查询点存在于表内)
private:
	//内存分配对象
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //自动的B2AABB对象管理
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//自动的生成视图对象管理的对象 
	//当前所有的距离对象
	ViewRangeRecordMap m_AxisBodyMap[AxisType::MAX_AXIS];//其中有X坐标列和Y坐标列 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//角色对应的范围信息 
	std::unordered_set<ActorID> m_DelayCalcMoveList;//角色对应的范围信息
}; 



void ViewRange::ClearObserverTable() //清理当前观察到的所有角色
{
	m_ObserverMap.clear();
}

const b2Vec2& ViewRange::GetViewRange() //获取到当前的视图范围
{
	return m_ObserverRange;
}
const b2Vec2& ViewRange::GetBodyPos()//获取到当前玩家的点位
{
	return m_BodyPos;
}
void ViewRange::LeaveView(ActorID actorID) //某一个角色离开了视图
{
	m_ObserverMap.erase(actorID);
}
const b2AABB& ViewRange::GetBodyAABB(PointType type) const//获取到当前角色的刚体AABB 
{
	return m_BodyAABB[type];//获取到某一类型的AABB盒子
}
const std::list<b2AABB*>& ViewRange::GetSplitAABB(PointType type)//获取到当前的裁剪AABB
{
	return m_SplitAABBList[type];
}

void ViewRange::RecalcBodyAABB(PointType type)//重新计算视口
{
	RecalcAABBRange(type);//计算基础的AABB
	CalcSplitAABB(type);//计算切割后的AABB
}

void ViewRange::RecalcBodyPosition()
{
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}
void ViewRange::RecalcRefreshCondtion()//重计算刷新条件
{	//角色移动了相对于角色当前百分之10的位置的话
	m_OffsetCondtion = m_BodyAABB[PointType::BODY_TYPE].upperBound - m_BodyAABB[PointType::BODY_TYPE].lowerBound;
	m_OffsetCondtion *= 0.1;
}
BodyData* ViewRange::GetActor() {//获取到当前的角色
	return m_Actor;
}

int ViewRange::IsRefreshActorView()//计算视差,大于多少才会重计算
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//获取到当前的位置 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return 1;
	return 0;
}
bool ViewRange::EnterView(ViewRange* actor)//某一个角色进入视野
{
	m_ObserverMap[actor->GetActor()->ID()] = actor;//我观察到了别人
	actor->EnterBeObseverView(this);//别人被我观察到了
}
const std::unordered_map<ActorID, ViewRange*>& ViewRange::GetVisibleMap()//获取到所有可见角色
{
	return this->m_ObserverMap;
}

bool ViewRange::EnterBeObseverView(ViewRange* actor)//某一个角色进入到了被观察的视图
{
	m_BeObserverMap[actor->GetActor()->ID()] = actor;
}

void ViewRange::RecycleAABBPoint(PointType type)//回收点位
{
	for (auto item = m_SplitAABBList->begin(); item != m_SplitAABBList->end();) {
		m_AABBAllocObj->BackObj(*item);//首先返回当前的信息
		m_SplitAABBList->erase(item++);//删除当前这个
	}
}

void AxisDistanceManager::AddDelayCalcTable(ActorID id) {
	m_DelayCalcMoveList.insert(id);//这个角色将再下一帧被重计算
}

void AxisDistanceManager::UnregisterBody(ActorID id)
{
	if (!m_ViewObjMap.count(id)) return;//未注册过
	ViewRange* rangeObj = m_ViewObjMap[id];//获取到对象
	RemoveDistanceAABB(rangeObj, PointType::BODY_TYPE);//删除到当前的AABb信息
	RemoveDistanceAABB(rangeObj, PointType::VIEW_TYPE);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
bool AxisDistanceManager::RegisterBody(BodyData* actor) {
	ActorID actorID = actor->ID();
	if (m_ViewObjMap.count(actorID)) return false;//对象已经注册  
	m_ViewObjMap[actorID] = new ViewRange(actor, &m_AABBIdleGenerate);//注册进入即可
	AddDelayCalcTable(actorID);//在下一帧重新计算角色信息
	return true;
}
//重新计算角色距离
void AxisDistanceManager::RecalcActorDistance() {
	ActorsMove();//更新所有角色的视距,包括碰撞AABB 
	CalcViewObj();//计算角色可以观察到的视图对象 
}

void AxisDistanceManager::RemoveDistanceAABB(ViewRange* actoViewRange, PointType type){
	int index = 1;
	auto & splitAABBList = actoViewRange->GetSplitAABB(type);//获取到当前裁切后的AABB
	ActorID actorID = actoViewRange->GetActor()->ID();//获取到角色的ID
	for (auto aabbItem = splitAABBList.begin(); aabbItem != splitAABBList.end(); aabbItem++ ) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//获取到当前的区域ID
		b2AABB* aabb = *aabbItem;
		RemoveDistancePoint(actorID, type,*aabb);//删除原有的物理碰撞盒子
		index++;
	}
}
void AxisDistanceManager::AdditionDistanceAABB(ViewRange* actoViewRange, PointType type) {
	const std::list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(type);//获取到当前裁切后的AABB
	int index = 1;
	ActorID actorID = actoViewRange->GetActor()->ID();
	for (auto item = splitAABBList.begin(); item != splitAABBList.end(); item++, index++) {
		ActorID reActorID = GEN_AABB_ID(actorID, index);//获取到当前的区域ID
		AdditionDistancePoint(actorID, type, *(*item));//删除原有的物理碰撞盒子
	}
}
void AxisDistanceManager::ABBBRnageRecycle(float pos, ViewRangeRecordMap& axisMap)
{
	if (!axisMap.count(pos)) return;//不存在的话
	ViewRangeTypeSet* rangeTypeSet = axisMap[pos];
	for (int type = 0; type < MAX_POINT_TYPE; type++)//循环遍历视图点位 和 最大点位是否都为空
		if (!(*rangeTypeSet)[(PointType)type].empty())
			return;
	m_ViewCellIdleGenerate.BackObj(rangeTypeSet);//回收它
	axisMap.erase(pos);
}

inline AxisDistanceManager& AxisDistanceManager::Instance() {
	static AxisDistanceManager Instance;
	return Instance;
}
ViewRange* AxisDistanceManager::GetViewRange(ActorID actorID)
{
	return m_ViewObjMap[actorID];
}