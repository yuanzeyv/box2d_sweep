#pragma once
#include "Manager/Base/BodyData.h"
#include "Manager/BodyManager.h" 
#include <map>
#include <list>     
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include "Define.h"     
#define MAX_OVERFLOW_RANGE (0.5f) //按照1米等分 
//bit 56-64 剩余类型
//bit 48-55 区域ID
//bit 0-54  角色ID
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

class ViewRangeTypeSet {
public:
	std::unordered_set<ActorID> m_TypeSet[PointType::MAX_POINT_TYPE];
	ViewRangeTypeSet() {}
	inline std::unordered_set<ActorID>& operator[](PointType type) {
		return m_TypeSet[type];
	}
};
typedef std::map<float, ViewRangeTypeSet*> ViewRangeRecordMap;

   
template<typename ListType>
class AutomaticGenerator {
public:
	AutomaticGenerator(int allocStep = 100) :m_AllocStep(allocStep), m_AllocCount(0), m_IdleList() {}
	AutomaticGenerator::~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size()) 
			printf("当前有%s对象%lld个未被回收,将造成内存泄漏,请检查代码逻辑\n\r", typeid(ListType).name(), m_AllocCount - m_IdleList.size());
		for (auto item = m_IdleList.begin(); item != m_IdleList.end(); item++)	delete* item;
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //全部清理
	}
	ListType* RequireObj() { 
		if (m_IdleList.size() == 0)//列表空了的话
			GenerateIdelListTypes();//1000个角色  
		auto beginObj = m_IdleList.begin();//获取到第一个
		ListType* listTypeObj = *beginObj;//获取到第一个的值
		m_IdleList.erase(beginObj);//删除第一个
		return listTypeObj;
	}
	void BackObj(ListType* obj) {//归还一个指针
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++)
			m_IdleList.push_back(new ListType());//创建这么多个空闲对象
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//分配步长
	int m_AllocCount;//分配的个数
	std::std::list<ListType*> m_IdleList;//当前空闲的个数
};

//当前的视图范围
class ViewRange{
public:
	ViewRange(BodyData* bodyData, AutomaticGenerator<b2AABB>* allocObj, b2Vec2 range = b2Vec2(0.5, 0.5));//视图的初始化
	inline BodyData* GetActor(); //获取到当前的角色 
	inline const b2Vec2& GetViewRange(); //获取到当前的观察范围
	inline const b2Vec2& GetBodyPos();//获取到当前玩家的点位 

	inline bool EnterView(ViewRange* actor);//某一个角色进入视野 
	inline void LeaveView(ActorID actorID); //某一个角色离开了视图 
	inline bool EnterBeObseverView(ViewRange* actor);//某一个角色进入到了被观察的视图

	inline bool InquiryBeObserver(ViewRange* actor); //计算制定AABB是包含自己
	inline void ClearObserverTable();//清理当前观察到的所有角色

	inline int JudgeActorViewAction();//计算视差,大于多少才会重计算 
	inline void MoveSelf(); //通知每个观察到自己的人,当前自己移动了  
	inline void RecalcBodyPosition();//重新获取到角色上一次未刷新的位置

	inline const b2AABB& GetBodyAABB(PointType type);//获取到当前角色的刚体AABB  
	inline const std::list<b2AABB*>& GetSplitAABB(PointType type);//获取到当前的裁剪AABB 
	inline void RecalcAABBRange(PointType type);//重新计算视口 
	inline void CalcSplitAABB(PointType type);//计算分割刚体(AABB会经常算,视图不经常算)
	inline void RecalcBodyAABB(PointType type);//重新计算视口
	inline void RecycleAABBPoint(PointType type);//回收点位 
	~ViewRange();  
private:
	BodyData* m_Actor;//当前的角色 
	std::unordered_map<ActorID, ViewRange*> m_ObserverMap;//可以看到的角色列表  
	std::unordered_map<ActorID, ViewRange*> m_BeObserverMap;//用于记录每个角色被谁观察到 
	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围  
	b2Vec2 m_BodyPos;//上一次的角色位置
	b2AABB m_BodyAABB[PointType::MAX_POINT_TYPE];//记录角色上一次添加时的AABB     
	std::list<b2AABB*> m_SplitAABBList[PointType::MAX_POINT_TYPE];//最终的,分割后的AABB
	AutomaticGenerator<b2AABB>* m_AABBAllocObj;//保存一下分配AABB的对象
}; 

class AxisDistanceManager
{
public:  
	inline void AddtionCalcBody(ActorID id);
	inline bool RegisterBody(ActorID id);
	//重新计算角色距离
	void RecalcActorDistance();
	inline void RemoveDistanceAABB(ViewRange* actoViewRange, PointType type);
	//在使用本函数之前应确保,当前的列表中,再无对应角色
	inline void AdditionDistancePoint(ActorID actorID, PointType addType, const b2AABB& viewRange);
	inline void RemoveDistancePoint(ActorID actorID, PointType type, const b2AABB& viewRange);
	inline void ABBBRnageRecycle(float pos, bool isX);
	void MoveActor();
	//寻找某一个轴的某一范围内的所有单元信息
	bool InquiryAxisPoints(std::unordered_set<ActorID>& outData, float findKey, PointType type, AxisType axis, float inquiryDistance, float leftOffset = 0, float rightOffset = 0);//偏移单元代表当前要查询的信息(必须确保当前被查询点存在于表内)
	//重计算
	inline void CalcViewObj();
	void UnregisterBody(ActorID id);
private:
	//内存分配对象
	AutomaticGenerator<b2AABB> m_AABBIdleGenerate; //自动的B2AABB对象管理
	AutomaticGenerator<ViewRangeTypeSet> m_ViewCellIdleGenerate;//自动的生成视图对象管理的对象 
	//当前所有的距离对象
	ViewRangeRecordMap m_AxisBodyMap[AxisType::MAX_AXIS];//其中有X坐标列和Y坐标列 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//角色对应的范围信息 
	std::unordered_set<ActorID> m_DelayCalcMoveList;//角色对应的范围信息
	AxisDistanceManager() {}
}; 
