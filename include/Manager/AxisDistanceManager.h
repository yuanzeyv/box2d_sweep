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

#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | (type << 48)) //生成一个设置点位类型的数组
#define GEN_AABB_AREA_ID(actorID,id) ((actorID) | (type << 48)) //生成一个设置点位类型的数组
enum PointPosType {
	POS_BODY_LIMIT_MAX = 0,//点位最大
	POS_BODY_LIMIT_MIN = 1,//点位最小 
};
enum PointType{
	Body_Type = 0,
	View_Type = 1,
	MAX_POINT_TYPE = 2,
};

typedef unordered_set<ActorID> ViewRangeTypeSet[2];
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
			GenerateIdelRangeMap(m_AllocStep);//1000个角色  
		auto beginObj = m_IdleList.begin();
		ListType* listTypeObj = *beginObj;
		m_IdleList.erase(beginObj);
		return *listTypeObj;
	}
	void BackObj(ListType* obj) {
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++) {
			ListType* typeObj = new ListType;
			m_IdleList.insert(typeObj);//创建这么多个空闲对象
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
	b2AABB m_BodyAABB;//记录角色上一次添加时的AABB    
	b2AABB m_ViewAABB;//记录上一次添加时的视图AABB
	list<b2AABB*> SplitAABBList;//分割后的AABB 
	list<b2AABB*> SplitViewAABBList;//分割后的AABB 
	AutomaticGenerator<b2AABB>* m_AABBAllocObj;
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
	inline const b2AABB& GetBodyAABB()
	{
		return m_BodyAABB;
	}
	inline const b2AABB& GetViewAABB()
	{
		return m_ViewAABB;
	}
	//重新计算视口
	inline void RecalcViewRange()
	{
		m_BodyPos = m_Actor->GetBody()->GetPosition();
		m_ViewAABB.lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);
		m_ViewAABB.upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
	}
	inline bool IsSplitAABB()//是否分割了刚体
	{
		return (m_BodyAABB.upperBound.x - m_BodyAABB.lowerBound.x) > MAX_OVERFLOW_RANGE;
	}
	inline bool IsSplitViewAABB()//是否分割了刚体
	{
		return (m_ViewAABB.upperBound.x - m_ViewAABB.lowerBound.x) > MAX_OVERFLOW_RANGE;
	} 
	//计算分割刚体(AABB会经常算,视图不经常算)
	inline void CalcSplitAABB()//计算分割AABB
	{
		//首先获取到当前刚体的AABB的长度
		float length = m_BodyAABB.upperBound.x - m_BodyAABB.lowerBound.x;
		int count = ceil(length / MAX_OVERFLOW_RANGE);//计算需要分配多少个  
		int splitCount = count - SplitAABBList.size();//获取到当前的分割个数
		if (splitCount < 0) {
			for (int i = 0; i > splitCount; i--) { 
				auto obj = SplitAABBList.begin();
				m_AABBAllocObj->BackObj(*obj);
				SplitAABBList.erase(obj);
			} 
		} else if (splitCount > 0) {
			for (int i = 0; i > splitCount; i--) {
				auto obj = SplitAABBList.begin();
				SplitAABBList.push_back(m_AABBAllocObj->RequireObj());
			}
		} else {//出错了
			return;
		}
		float calcX = m_BodyAABB.lowerBound.x;
		for (auto item = SplitAABBList.begin(); item != SplitAABBList.end(); item++)
		{
			b2AABB* aabb = *item;
			aabb->lowerBound.x = calcX;
			aabb->lowerBound.y = m_BodyAABB.lowerBound.y;
			if (calcX + MAX_OVERFLOW_RANGE > m_BodyAABB.upperBound.x)
				aabb->upperBound.x = m_BodyAABB.upperBound.x;
			else {
				calcX += MAX_OVERFLOW_RANGE;
				aabb->upperBound.x = m_BodyAABB.upperBound.y;
			} 
		}
	}
	inline void CalcSplitViewAABB()//计算分割AABB
	{
		//首先获取到当前刚体的AABB的长度
		float length = m_ViewAABB.upperBound.x - m_ViewAABB.lowerBound.x;
		int count = ceil(length / MAX_OVERFLOW_RANGE);//计算需要分配多少个  
		int splitCount = count - SplitAABBList.size();//获取到当前的分割个数
		if (splitCount < 0) {
			for (int i = 0; i > splitCount; i--) {
				auto obj = SplitAABBList.begin();
				m_AABBAllocObj->BackObj(*obj);
				SplitAABBList.erase(obj);
			}
		}
		else if (splitCount > 0) {
			for (int i = 0; i > splitCount; i--) {
				auto obj = SplitAABBList.begin();
				SplitAABBList.push_back(m_AABBAllocObj->RequireObj());
			}
		}
		else {//出错了
			return;
		}
		float calcX = m_ViewAABB.lowerBound.x;
		for (auto item = SplitAABBList.begin(); item != SplitAABBList.end(); item++)
		{
			b2AABB* aabb = *item;
			aabb->lowerBound.x = calcX;
			aabb->lowerBound.y = m_ViewAABB.lowerBound.y;
			if (calcX + MAX_OVERFLOW_RANGE > m_ViewAABB.upperBound.x)
				aabb->upperBound.x = m_ViewAABB.upperBound.x;
			else {
				calcX += MAX_OVERFLOW_RANGE;
				aabb->upperBound.x = m_ViewAABB.upperBound.y;
			}
		}
	}
	//计算分割视图刚体
	//重新计算视口
	inline void RecalcBodyAABB()
	{
		m_BodyAABB = m_Actor->GetAABB();
	}

	inline void RecalcBodyPosition()
	{
		m_BodyPos = m_Actor->GetBody()->GetPosition();
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
	void SetBodyPos(const b2Vec2& pos)
	{ 
		m_BodyPos.Set(pos.x, pos.y);
	}
	const b2Vec2& GetBodyPos()
	{
		return m_BodyPos;
	}
	~ViewRange()
	{ 
	}
};
class AxisDistanceManager
{
public:
	//当前所有的距离对象
	ViewRangeRecordMap m_XAxisBodyMap;//x坐标的角色记录 (记录顶点的左 右)
	ViewRangeRecordMap m_YAxisBodyMap;//x坐标的角色记录 (记录顶点的左 右)  

	unordered_map<ActorID, ViewRange*> m_ViewRangeMap;//角色对应的范围信息

	  
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //自动的B2AABB对象管理
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//自动的生成视图对象管理的对象

	unordered_set<ActorID> m_DelayCalcMoveList;//角色对应的范围信息
	AxisDistanceManager() {
	}  
	inline void AddtionCalcBody(ActorID id){
		m_DelayCalcMoveList.insert(id);//这个角色将再下一帧被重计算
	}

	inline bool RegisterBody(ActorID id){
		if (m_ViewRangeMap.count(id))//对象已经注册
			return false;
		BodyData* bodyData = BodyManager::Instance().GetBodyData(id);//通过身体控制单元获取到对应的身体
		ViewRange* viewRangeObj = new ViewRange(bodyData);
		m_ViewRangeMap[id] = viewRangeObj;//注册进入即可,程序会在下一帧的开始来移动角色
		AddtionCalcBody(id);
		return true;
	}
	void RecalcActorDistance()
	{
		MoveActor();//移动角色
		CalcViewObj();//计算角色可以观察到的视图对象 
	}
	//计算距离分两步,先整体移动玩角色后,再计算距离
	void MoveActor()
	{
		ViewRange* actoViewange = NULL;
		for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) {
			//首先计算当前的操作 
			ActorID actorID = *item;
			if (!m_ViewRangeMap.count(actorID)) {
				goto erase;
			}
			actoViewange = m_ViewRangeMap[actorID];
			if (actoViewange->JudgeActorViewAction() == 1) {//删除不需要计算的单元
				goto erase;
			} 
			//对当前角色进行位移,移动视野  移动碰撞盒
			RemoveDistancePoint(actorID, PointType::Body_Type, actoViewange->GetBodyAABB());//删除原有的物理碰撞盒子
			RemoveDistancePoint(actorID, PointType::View_Type, actoViewange->GetViewAABB());//删除原有的石头碰撞盒子
			actoViewange->RecalcViewRange();//重计算AABB
			actoViewange->RecalcBodyAABB();//重计算视图AABB
			actoViewange->RecalcBodyPosition();//重计算角色的位置
			AdditionDistancePoint(actorID, PointType::Body_Type, actoViewange->GetBodyAABB());//将现有的添加上去
			AdditionDistancePoint(actorID, PointType::View_Type, actoViewange->GetViewAABB());//将现有的添加上去 
			item = item++; 
		erase:
			m_DelayCalcMoveList.erase(actorID);
		} 
	}
	//重计算
	inline void CalcViewObj()
	{	//经过上一次筛选后,留下来的被移动的单元set
		for (auto item = m_DelayCalcMoveList.begin();item != m_DelayCalcMoveList.end();) { 
			ActorID actorID = *item;//首先计算当前的操作 
			ViewRange* actoViewange = m_ViewRangeMap[actorID];//获取到角色的使徒信息
			actoViewange->ClearObserverTable();//清理当前的可见对象
			const b2AABB& bodyViewAABB = actoViewange->GetViewAABB();//获取到角色的视图范围
			unordered_set<ActorID> visibleSet;
			auto xBegin = m_XAxisBodyMap[bodyViewAABB.lowerBound.x];//从低一直找到高
			//寻找视图范围内的所有节点
			for (auto xBegin = m_XAxisBodyMap.find(bodyViewAABB.lowerBound.x);xBegin != m_XAxisBodyMap.end();xBegin++) {
				if (xBegin->first > bodyViewAABB.upperBound.x)
					break;
				unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.x][PointType::Body_Type];
				if (actorList->size() == 0)
					continue; 
				for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
					visibleSet.insert(*objItem);
				} 
			}
			//寻找y轴
			for (auto yBegin = m_YAxisBodyMap.find(bodyViewAABB.lowerBound.y);yBegin != m_YAxisBodyMap.end();yBegin++) {
				if (yBegin->first > bodyViewAABB.upperBound.y)
					break;
				unordered_set<ActorID>* actorList = m_XAxisBodyMap[bodyViewAABB.lowerBound.y][PointType::Body_Type];
				if (actorList->size() == 0) 
					continue; 
				for (auto objItem = actorList->begin();objItem != actorList->end();objItem++) {
					if (visibleSet.count(*objItem) == 0)
						continue;
					visibleSet.erase(*objItem);
					actoViewange->EnterView(*objItem);
				}
			} 
		}   
	}

	inline void RemoveDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
	{
		if (m_XAxisBodyMap.count(viewRange.lowerBound.x))//存在当前坐标点的话 
			if (m_XAxisBodyMap[viewRange.lowerBound.x][addType]->count(actorID))
				m_XAxisBodyMap[viewRange.lowerBound.x][addType]->erase(actorID);
		if (m_XAxisBodyMap.count(viewRange.upperBound.x))//存在当前坐标点的话 
			if (m_XAxisBodyMap[viewRange.upperBound.x][addType]->count(actorID))
				m_XAxisBodyMap[viewRange.upperBound.x][addType]->erase(actorID);
		if (m_YAxisBodyMap.count(viewRange.lowerBound.y))//存在当前坐标点的话 
			if (m_YAxisBodyMap[viewRange.lowerBound.y][addType]->count(actorID))
				m_YAxisBodyMap[viewRange.lowerBound.y][addType]->erase(actorID);
		if (m_YAxisBodyMap.count(viewRange.upperBound.y))//存在当前坐标点的话 
			if (m_YAxisBodyMap[viewRange.upperBound.y][addType]->count(actorID))
				m_YAxisBodyMap[viewRange.upperBound.y][addType]->erase(actorID);
		//判断是否使用结束了
		ViewRnageUserOver(viewRange.lowerBound.x, true, true);
		ViewRnageUserOver(viewRange.upperBound.x, true, true);
		ViewRnageUserOver(viewRange.lowerBound.y, false, true);
		ViewRnageUserOver(viewRange.upperBound.y, false, true);
	}
	//在使用本函数之前应确保,当前的列表中,再无对应角色
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange)
	{ 
		//判断有无,申请
	    //if (!m_XAxisBodyMap.count(viewRange.lowerBound.x)){//不存在当前坐标点的话  
	    //	ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
	    //	m_XAxisBodyMap[viewRange.lowerBound.x] = typeSet;
	    //}
	    //if (!m_XAxisBodyMap.count(viewRange.upperBound.x)){//不存在当前坐标点的话  
	    //	ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
	    //	m_XAxisBodyMap[viewRange.upperBound.x] = typeSet;
	    //}
	    //if (!m_YAxisBodyMap.count(viewRange.lowerBound.y)) {//不存在当前坐标点的话  
	    //	ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
	    //	m_YAxisBodyMap[viewRange.lowerBound.y] = typeSet;
	    //}
	    //if (!m_YAxisBodyMap.count(viewRange.upperBound.y)) {//不存在当前坐标点的话  
	    //	ViewRangeTypeSet* typeSet = AllocRangeTypeSet();
	    //	m_YAxisBodyMap[viewRange.upperBound.y] = typeSet;
	    //}
	    //m_XAxisBodyMap[viewRange.lowerBound.x][addType]->insert(actorID);
	    //m_XAxisBodyMap[viewRange.upperBound.x][addType]->insert(actorID);
	    //m_YAxisBodyMap[viewRange.lowerBound.y][addType]->insert(actorID);
	    //m_YAxisBodyMap[viewRange.upperBound.y][addType]->insert(actorID); 
	}

	inline void ViewRnageUserOver(float pos,bool isX,bool virify = true)
	{
		ViewRangeRecordMap& tempMap = isX?m_YAxisBodyMap: m_YAxisBodyMap; 
		if (virify && tempMap.count(pos) == 0)
			return;
		ViewRangeTypeSet* rangeTypeSet = tempMap[pos];
		bool isEmpty = true;
		for (int i = 0;i < MAX_POINT_TYPE;i++) {
			if (!rangeTypeSet[i]->empty()) {
				isEmpty = false;
				break;
			}
		}
		if (!isEmpty)
			return;
		//m_IdleViewRangeMap.push_back(rangeTypeSet);
		tempMap.erase(pos); 
	}
	void UnregisterBody(ActorID id)
	{
		if (m_ViewRangeMap.count(id))
			return ; //已经注册过了 
		ViewRange* rangeObj = m_ViewRangeMap[id];
		RemoveDistancePoint(id, PointType::Body_Type, rangeObj->GetBodyAABB());
		RemoveDistancePoint(id, PointType::View_Type, rangeObj->GetViewAABB());
		delete m_ViewRangeMap[id];
		m_ViewRangeMap.erase(id);
	} 
	 
}; 
