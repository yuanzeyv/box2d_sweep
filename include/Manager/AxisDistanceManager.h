#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "Define.h"
#include <list>     
#include <math.h> 
#include <typeinfo>
//当前角色可以看到谁
//当前谁可以看到角色
using std::unordered_map;
using std::map;
using std::unordered_set;
using std::list;
using std::vector;
#define MAX_OVERFLOW_RANGE (0.5f) //按照1米等分 
/*
bit 56-64 剩余类型
bit 48-55 区域ID
bit 0-54  角色ID
*/
#define AREA_ID(actorID) (((actorID) & (0xff << 48)) >> 48) //取得当前的区域ID
#define AABB_POINT_TYPE(actorID) ((actorID) & (0x1 << 48) > 0) //当前的点位类型

#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ( (type & 1) << 56)) //生成一个设置点位类型的数组 
#define GEN_AABB_ID(actorID,index) (actorID | ((index & 0xffff) << 48 ))//生成一个ID
enum PointPosType {
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
class ViewRangeTypeSet {
public:
	unordered_set<ActorID> m_TypeSet[PointType::MAX_POINT_TYPE];
	ViewRangeTypeSet() {}
	unordered_set<ActorID>& operator[](PointType type) {
		return m_TypeSet[type];
	}
	unordered_set<ActorID>& GetRangeType(PointType type) {
		return m_TypeSet[type];
	}
}; 
typedef map<float, ViewRangeTypeSet*> ViewRangeRecordMap;
   
template<typename ListType>
class AutomaticGenerator {
public:
	AutomaticGenerator() :m_AllocStep(100) {
	}
	~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size()) {
			printf("当前有%s对象%lld个未被回收,将造成内存泄漏,请检查代码逻辑\n\r", typeid(ListType).name(), m_AllocCount - m_IdleList.size());
		}
		for (auto item = m_IdleList.begin(); item != m_IdleList.end(); item++)
			delete* item;//删除它 
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //全部清理
	}
	ListType* RequireObj() {
		if (m_IdleList.size() == 0)//列表空了的话
			GenerateIdelListTypes();//1000个角色  
		auto beginObj = m_IdleList.begin();
		ListType* listTypeObj = *beginObj;
		m_IdleList.erase(beginObj);
		return listTypeObj;
	}
	void BackObj(ListType* obj) {
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++) {
			ListType* typeObj = new ListType();
			m_IdleList.push_back(typeObj);//创建这么多个空闲对象
		}
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//分配步长
	int64_t m_AllocCount;//分配的个数
	std::list<ListType*> m_IdleList;//当前空闲的个数
};
class ViewRange {
public: 
	BodyData*  m_Actor;//当前的角色 
	unordered_set<ActorID> m_ObserverMap;//可以看到的角色列表 
	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围  
	 
	b2Vec2 m_BodyPos;//上一次的角色位置
	b2AABB m_BodyAABB[PointType::MAX_POINT_TYPE];//记录角色上一次添加时的AABB     
	list<b2AABB*> m_SplitAABBList[PointType::MAX_POINT_TYPE];//最终的,分割后的AABB

	AutomaticGenerator<b2AABB>* m_AABBAllocObj;//保存一下分配AABB的对象
public:
	ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj) {
		Reset();//清理一下数据
		m_Actor = bodyData;
		m_AABBAllocObj = allocObj;  
	}
	inline void Reset()
	{
		m_Actor = NULL;
		m_ObserverRange.Set(0.5, 0.5);
		m_BodyPos.Set(MAX_DISTANCE, MAX_DISTANCE);
		m_ObserverMap.clear(); 
	} 

	//获取到当前角色的刚体AABB 
	inline const b2AABB& GetBodyAABB(PointType type)
	{
		return m_BodyAABB[type];
	} 
	//获取到当前的视图列表
	const list<b2AABB*>& GetSplitAABB(PointType type)
	{ 
		return m_SplitAABBList[type];
	}
	//重新计算视口
	inline void RecalcAABBRange(PointType type)
	{
		if(type == PointType::VIEW_TYPE){
			m_BodyPos = m_Actor->GetBody()->GetPosition();
			m_BodyAABB[type].lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);
			m_BodyAABB[type].upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
		} else {
			const b2AABB& bodyAABB = m_Actor->GetAABB();
			m_BodyAABB[type].lowerBound.Set(bodyAABB.lowerBound);
			m_BodyAABB[type].upperBound.Set(bodyAABB.upperBound); 
		}
	}
	inline void RecalcBodyPosition()
	{
		m_BodyPos = m_Actor->GetBody()->GetPosition();
	} 
	//计算分割刚体(AABB会经常算,视图不经常算)
	inline void CalcSplitAABB(PointType type)//计算分割AABB
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
		} else if (splitCount > 0) {
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
				aabb->upperBound.Set(calcX + MAX_OVERFLOW_RANGE,m_BodyAABB[type].upperBound.y);
			else
				aabb->upperBound.Set(m_BodyAABB[type].upperBound.y, m_BodyAABB[type].upperBound.y); 
		}
	} 
	//计算分割视图刚体
	//重新计算视口
	inline void RecalcBodyAABB(PointType type)
	{
		RecalcAABBRange(type);//计算基础的AABB
		CalcSplitAABB(type);//计算切割后的AABB
	}

	//清理当前观察到的所有角色
	inline void ClearObserverTable() {
		m_ObserverMap.clear();
	} 
	inline const b2Vec2& GetViewRange() 
	{
		return m_ObserverRange;
	}
	//1代表当前仅添加视差
	//2代表当前计算视差 
	inline int JudgeActorViewAction()
	{
		b2Vec2 bodyPos = m_Actor->GetBody()->GetPosition();//获取到当前的位置 
		float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
		float yMinus = std::abs(bodyPos.x - m_BodyPos.x);
		float xPercent = xMinus / (m_ObserverRange.x * 2);
		float yPercent = yMinus / (m_ObserverRange.y * 2);
		float finalPercent = std::max(xPercent, yPercent);
		if (finalPercent <= 3) {//视差任意轴移动距离小于百分之3,不做计算 
			return 0;
		}
		return 1;
	}
	//一组人进视野
	bool EnterView(ActorID actorID)
	{
		m_ObserverMap.insert(actorID);
	}
	void LeaveView(ActorID actorID) 
	{
		m_ObserverMap.erase(actorID);
	} 
	const b2Vec2& GetBodyPos()
	{
		return m_BodyPos;
	}
	//回收点位
	void RecycleAABBPoint(PointType type)
	{
		auto item = m_SplitAABBList->begin();
		while (item != m_SplitAABBList->end()) {
			m_AABBAllocObj->BackObj(*item);
			item = m_SplitAABBList->erase(item);
		}
	}
	~ViewRange()
	{
		RecycleAABBPoint(PointType::BODY_TYPE);
		RecycleAABBPoint(PointType::VIEW_TYPE);
	}
};
//用观察这模式,来实现当某一角色移动后,观察到的玩家,需要做某些事情
//有一个中介这模式,用于通知所有的观察者
class 

class AxisDistanceManager
{
public:  
	//内存分配对象
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //自动的B2AABB对象管理
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//自动的生成视图对象管理的对象 
	//当前所有的距离对象
	ViewRangeRecordMap m_AxisBodyMap[AxisType::MAX_AXIS];//其中有X坐标列和Y坐标列 
	unordered_map<ActorID, ViewRange*> m_ViewObjMap;//角色对应的范围信息

	unordered_set<ActorID> m_DelayCalcMoveList;//角色对应的范围信息
	AxisDistanceManager(){}
	inline void AddtionCalcBody(ActorID id){
		m_DelayCalcMoveList.insert(id);//这个角色将再下一帧被重计算
	} 
	inline bool RegisterBody(ActorID id) {
		if (m_ViewObjMap.count(id)) return false;//对象已经注册 
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//通过身体控制单元获取到对应的身体 
		m_ViewObjMap[id] = new ViewRange(bodyData, &m_AABBIdleGenerate);//注册进入即可
		AddtionCalcBody(id);//在下一帧重新计算角色信息
		return true;
	}
	//重新计算角色距离
	void RecalcActorDistance(){
		//整体计算一下当前的移动距离
		MoveActor();//更新所有角色的视距,包括碰撞AABB 
		CalcViewObj();//计算角色可以观察到的视图对象 
	}
	inline void RemoveDistanceAABB(ViewRange* actoViewRange, PointType type) {
		const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//获取到当前裁切后的AABB
		int index = 1;
		ActorID actorID = actoViewRange->m_Actor->ID();
		for (auto item = splitAABBList.begin();item != splitAABBList.end();item++, index++) {
			ActorID reActorID = GEN_AABB_ID(actorID, index);//获取到当前的区域ID
			RemoveDistancePoint(actorID, type, *(*item));//删除原有的物理碰撞盒子
		}
	}
	inline void AdditionDistanceAABB(ViewRange* actoViewRange, PointType type) {
		const list<b2AABB*>& splitAABBList = actoViewRange->GetSplitAABB(PointType::BODY_TYPE);//获取到当前裁切后的AABB
		int index = 1;
		ActorID actorID = actoViewRange->m_Actor->ID();
		for (auto item = splitAABBList.begin();item != splitAABBList.end();item++, index++) {
			ActorID reActorID = GEN_AABB_ID(actorID, index);//获取到当前的区域ID
			AdditionDistancePoint(actorID, type, *(*item));//删除原有的物理碰撞盒子
		}
	}
	//在使用本函数之前应确保,当前的列表中,再无对应角色
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
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
		xRangeRecordMap[viewRange.lowerBound.x]->GetRangeType(addType).insert(actorID);
		xRangeRecordMap[viewRange.upperBound.x]->GetRangeType(addType).insert(actorID);
		yRangeRecordMap[viewRange.lowerBound.y]->GetRangeType(addType).insert(actorID);
		yRangeRecordMap[viewRange.upperBound.y]->GetRangeType(addType).insert(actorID);
	}
	inline void RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange)
	{
		ActorID minActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MIN); //生成最小ID
		ActorID maxActorID = GEN_AABB_POINT_TYPE(actorID, PointPosType::POS_BODY_LIMIT_MAX); //生成最大ID 
		ViewRangeRecordMap& xRangeRecordMap = m_AxisBodyMap[AxisType::X_AXIS];
		ViewRangeRecordMap& yRangeRecordMap = m_AxisBodyMap[AxisType::Y_AXIS];
		if (xRangeRecordMap.count(viewRange.lowerBound.x))//删除这个ID对应的X坐标点
			if (xRangeRecordMap[viewRange.lowerBound.x]->GetRangeType(type).count(minActorID))
				xRangeRecordMap[viewRange.lowerBound.x]->GetRangeType(type).erase(minActorID);
		if (xRangeRecordMap.count(viewRange.upperBound.x))//存在当前坐标点的话 
			if (xRangeRecordMap[viewRange.upperBound.x]->GetRangeType(type).count(maxActorID))
				xRangeRecordMap[viewRange.upperBound.x]->GetRangeType(type).erase(maxActorID);
		if (yRangeRecordMap.count(viewRange.lowerBound.y))//存在当前坐标点的话 
			if (yRangeRecordMap[viewRange.lowerBound.y]->GetRangeType(type).count(minActorID))
				yRangeRecordMap[viewRange.lowerBound.y]->GetRangeType(type).erase(minActorID);
		if (yRangeRecordMap.count(viewRange.upperBound.y))//存在当前坐标点的话 
			if (yRangeRecordMap[viewRange.upperBound.y]->GetRangeType(type).count(maxActorID))
				yRangeRecordMap[viewRange.upperBound.y]->GetRangeType(type).erase(maxActorID);
		//判断是否使用结束了
		ABBBRnageRecycle(viewRange.lowerBound.x, true);
		ABBBRnageRecycle(viewRange.upperBound.x, true);
		ABBBRnageRecycle(viewRange.lowerBound.y, false);
		ABBBRnageRecycle(viewRange.upperBound.y, false);
	}
	inline void ABBBRnageRecycle(float pos, bool isX)
	{
		ViewRangeRecordMap& tempMap = isX ? m_AxisBodyMap[AxisType::X_AXIS] : m_AxisBodyMap[AxisType::Y_AXIS];
		if (!tempMap.count(pos))
			return;
		ViewRangeTypeSet* rangeTypeSet = tempMap[pos];
		bool isEmpty = true;
		for (int type = 0;type < MAX_POINT_TYPE;type++) {
			if (!rangeTypeSet->GetRangeType((PointType)type).empty()) {
				isEmpty = false;
				break;
			}
		}
		if (!isEmpty)
			return;
		m_ViewCellIdleGenerate.BackObj(rangeTypeSet);//回收它
		tempMap.erase(pos);
	}
	void MoveActor()
	{
		//移动角色,如果角色符合移动的条件,则进入下一步视距范围的判断
		ViewRange* actoViewange = NULL;
		bool OnlyCalculate = false;
		for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) {
			//首先计算当前的操作 
			ActorID actorID = *item;
			OnlyCalculate = true;
			if (!m_ViewObjMap.count(actorID))//对象不存在,直接删除
				goto erase; 
			actoViewange = m_ViewObjMap[actorID];
			if (actoViewange->JudgeActorViewAction() == 1){//移动距离过浅
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
	//重计算
	inline void CalcViewObj()
	{	

		//移动角色,分为三步
		//1:首先删除当前角色所能看到的所有玩家
		//1-1:重新计算,视图范围内,所看到的玩家数目

		//2:通知当前观察到该角色的玩家,角色发生了移动,然后进行修改
		
		//经过上一次筛选后,留下来的被移动的单元set
		//for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) {
		//	ActorID actorID = *item;//首先计算当前的操作 
		//	ViewRange* actoViewange = m_ViewObjMap[actorID];//获取到角色的使徒信息
		//	actoViewange->ClearObserverTable();//清理当前的可见对象
		//	const b2AABB& bodyViewAABB = actoViewange->GetViewAABB();//获取到角色的视图范围
		//	unordered_set<ActorID> visibleSet;
		//	auto xBegin = m_XAxisBodyMap[bodyViewAABB.lowerBound.x];//从低一直找到高
		//	//寻找视图范围内的所有节点
		//	for (auto xBegin = m_XAxisBodyMap.find(bodyViewAABB.lowerBound.x);xBegin != m_XAxisBodyMap.end();xBegin++) {
		//		if (xBegin->first > bodyViewAABB.upperBound.x)
		//			break;
		//		unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.x][PointType::Body_Type];
		//		if (actorList->size() == 0)
		//			continue;
		//		for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
		//			visibleSet.insert(*objItem);
		//		}
		//	}
		//	//寻找y轴
		//	for (auto yBegin = m_YAxisBodyMap.find(bodyViewAABB.lowerBound.y);yBegin != m_YAxisBodyMap.end();yBegin++) {
		//		if (yBegin->first > bodyViewAABB.upperBound.y)
		//			break;
		//		unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.y][PointType::Body_Type];
		//		if (actorList->size() == 0)
		//			continue;
		//		for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
		//			if (visibleSet.count(*objItem) == 0)
		//				continue;
		//			visibleSet.erase(*objItem);
		//			actoViewange->EnterView(*objItem);
		//		}
		//	}
		//}
	} 
	void UnregisterBody(ActorID id)
	{
		if (m_ViewObjMap.count(id))
			return ; //已经注册过了  
		ViewRange* rangeObj = m_ViewObjMap[id];
		RemoveDistanceAABB(rangeObj, PointType::BODY_TYPE);
		RemoveDistanceAABB(rangeObj, PointType::VIEW_TYPE); 
		delete rangeObj;
		m_ViewObjMap.erase(id);
	}  
}; 
