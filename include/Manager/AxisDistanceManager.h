#pragma once
#include "Manager/Base/BodyData.h" 
#include <map>
#include <list>     
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include "Define.h" 
#include "TemplateBoard.h"  
//bit 50-64 剩余类型
//bit 48-49 区域ID
//bit 00-47  角色ID
#define GEN_AABB_POINT_TYPE(actorID,type) ((actorID) | ((type & 3) << 47)) //生成最大最小点类型
#define GET_ACTOR_ID(actorID) (actorID & (~((ActorID)(0xffff) << 47)))//获取到当前的角色ID
enum PointPosType {//要不要应该都可以
	POS_BODY_LIMIT_LEFT_MAX = 0,//左上
	POS_BODY_LIMIT_LEFT_MIN = 1,//左下 
	POS_BODY_LIMIT_RIGHT_MAX = 2,//右上 
	POS_BODY_LIMIT_RIGHT_MIN = 3,//右下 
	POS_BODY_LIMIT_NUM_MAX = 4,//最大个数
}; 
enum PointType {
	BODY_TYPE = 0,
	VIEW_TYPE = 1,
	MAX_POINT_TYPE = 2,
};
struct CompareFloat {
	bool operator()(const b2Vec2& k1, const b2Vec2& k2) const {
		if (k1.x < k2.x)
			return true;
		if (k1.x > k2.x)
			return false;
		return k1.y < k2.y;
	}
}; 
class SmartActorIDMan {
public:
	inline void Insert(ActorID actorID, PointPosType type)
	{
		actorID = GEN_AABB_POINT_TYPE(actorID, type);//生成一个新的
		m_ActorSet.insert(actorID);
	}
	inline void Erease(ActorID actorID, PointPosType type)
	{
		actorID = GEN_AABB_POINT_TYPE(actorID, type);//生成一个新的
		m_ActorSet.erase(actorID);
	}
	inline const std::unordered_set<ActorID>& GetTable() {
		return  m_ActorSet;
		
	}
	inline void Init() {
		m_ActorSet.clear();
	}
	inline bool IsInValid() { 
		return m_ActorSet.size() == 0;
	}
private: 
	std::unordered_set<ActorID> m_ActorSet;//记录一组无序的角色ID
};
typedef std::map<b2Vec2, SmartActorIDMan*, CompareFloat > ViewRangeRecordMap;//视图轴

//当前的视图范围
class ViewRange{
public:
	ViewRange(BodyData* bodyData,b2Vec2 range = b2Vec2(0.5, 0.5));//视图的初始化
	BodyData* GetActor(); //获取到当前的角色 
	const b2Vec2& GetViewRange(); //获取到当前的观察范围
	const b2Vec2& GetBodyPos();//获取到当前玩家的点位 
	const std::unordered_map<ActorID, ViewRange*>& GetVisibleMap();//获取到所有可见角色
	const std::unordered_map<ActorID, ViewRange*>& GetBeObseverView(ViewRange* actor);//某一个角色进入到了被观察的视图
	const b2AABB& GetBodyAABB() const;//获取到当前角色的刚体AABB   
	const b2AABB& GetViewAABB() const;//获取到当前角色的刚体AABB 

	bool EnterView(ViewRange* actor);//某一个角色进入视野 
	void LeaveView(ActorID actorID); //某一个角色离开了视图 
	bool EnterBeObseverView(ViewRange* actor);//某一个角色进入到了被观察的视图

	bool InObserverContain(ViewRange* actor); //计算制定AABB是包含自己
	void ClearObserverTable();//清理当前观察到的所有角色

	int IsRefreshActorView();//计算视差,大于多少才会重计算 
	void MoveSelf(); //通知每个观察到自己的人,当前自己移动了  
	void RecalcBodyPosition();//重新获取到角色上一次未刷新的位置

	void RecalcBodyAABB();//重新计算视口 
	void RecalcViewAABB();//重新计算视口
	void RecalcRefreshCondtion();//重计算刷新条件    
	~ViewRange() {}
private:
	BodyData* m_Actor;//当前的角色 
	b2Vec2 m_BodyPos;//上一次的角色位置

	std::unordered_map<ActorID, ViewRange*> m_ObserverMap;//可以看到的角色列表  
	std::unordered_map<ActorID, ViewRange*> m_BeObserverMap;//用于记录每个角色被谁观察到 
	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围  

	b2AABB m_BodyAABB;//记录角色上一次添加时的AABB    
	b2AABB m_ViewAABB;//记录角色上一次添加时的AABB      

	b2Vec2 m_OffsetCondtion;//偏移极限 
}; 

class AxisDistanceManager
{
public:
	//注册列表
	bool RegisterBody(BodyData* actor);//注册一个对象到距离管理
	void UnregisterBody(ActorID id);//反注册
	void UnregisterBody(BodyData* actor);//反注册
	//计算区域 
	void AddDelayCalcTable(ActorID id);//添加一个刚体计算对象 
	void AdditionViewAABB(ViewRange* actoViewRange);//添加一个视图AABB
	void RemoveViewAABB(ViewRange* actoViewRange); 
	//获取到单元
	ViewRange* GetViewRange(ActorID actorID);
	//重计算
	void Update();//重新计算角色距离
	void ActorsMove();
	void CalcViewObj();//重计算

	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager() {}
	//寻找某一个轴的某一范围内的所有单元信息
	void InquiryAxisPoints(std::unordered_set<ActorID>& outData,const b2AABB& viewAABB);
private:
	//内存分配对象 
	AutomaticGenerator<SmartActorIDMan> m_ViewCellIdleGenerate;//自动的生成视图对象管理的对象 
	//当前所有的距离对象
	ViewRangeRecordMap m_AxisBodyMap;//其中有X坐标列和Y坐标列 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//角色对应的范围信息 
	std::unordered_set<ActorID> m_DelayCalcMoveList;//角色对应的范围信息
}; 

inline void ViewRange::ClearObserverTable() //清理当前观察到的所有角色
{  
	m_ObserverMap.clear();
}

inline const b2Vec2& ViewRange::GetViewRange() //获取到当前的视图范围
{
	return m_ObserverRange;
}
inline const b2Vec2& ViewRange::GetBodyPos()//获取到当前玩家的点位
{
	return m_BodyPos;
}
inline void ViewRange::LeaveView(ActorID actorID) //某一个角色离开了视图
{
	m_ObserverMap.erase(actorID);
}
inline const b2AABB& ViewRange::GetBodyAABB() const//获取到当前角色的刚体AABB 
{
	return m_BodyAABB;//获取到某一类型的AABB盒子
}
inline const b2AABB& ViewRange::GetViewAABB() const//获取到当前角色的刚体AABB 
{
	return m_ViewAABB;//获取到某一类型的AABB盒子
}

inline void ViewRange::RecalcViewAABB()//重新计算视口
{
	m_ViewAABB.lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);//重新计算视图
	m_ViewAABB.upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
}
inline void ViewRange::RecalcBodyAABB()//重新计算视口
{
	this->m_Actor->CalcBodyAABB();
	const b2AABB& bodyAABB = m_Actor->GetAABB();//获取到角色的AABB
	memcpy(&m_BodyAABB, &bodyAABB, sizeof(b2AABB));//拷贝数据
	RecalcRefreshCondtion(); 
}

inline void ViewRange::RecalcBodyPosition()
{ 
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}
inline void ViewRange::RecalcRefreshCondtion()//重计算刷新条件
{	//角色移动了相对于角色当前百分之10的位置的话
	m_OffsetCondtion = m_BodyAABB.upperBound - m_BodyAABB.lowerBound; 
	m_OffsetCondtion *= 0.1;
}
inline BodyData* ViewRange::GetActor() {//获取到当前的角色
	return m_Actor;
}

inline bool ViewRange::InObserverContain(ViewRange* actor) {//移动的角色询问当前是否还在监听范围内 
	if (!b2TestOverlap(m_ViewAABB, actor->m_BodyAABB)) {//如果当前看得观察到了这个刚体,但现在看不见了
		m_ObserverMap.erase(actor->m_Actor->ID());//执行删除
		return false;
	}
	return true;//删除自己
}
inline int ViewRange::IsRefreshActorView()//计算视差,大于多少才会重计算
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//获取到当前的位置 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.y - m_BodyPos.y);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return 1;
	return 0;
}
inline bool ViewRange::EnterView(ViewRange* actor)//某一个角色进入视野
{
	m_ObserverMap[actor->GetActor()->ID()] = actor;//我观察到了别人
	actor->EnterBeObseverView(this);//别人被我观察到了
	return true;
}
inline const std::unordered_map<ActorID, ViewRange*>& ViewRange::GetVisibleMap()//获取到所有可见角色
{
	return this->m_ObserverMap;
}

inline bool ViewRange::EnterBeObseverView(ViewRange* actor)//某一个角色进入到了被观察的视图
{
	if (m_BeObserverMap.count(actor->GetActor()->ID()))//存在就不创建
		return true;
	m_BeObserverMap[actor->GetActor()->ID()] = actor;
	return true;
}

inline const std::unordered_map<ActorID, ViewRange*>& ViewRange::GetBeObseverView(ViewRange* actor)//某一个角色进入到了被观察的视图
{
	return m_BeObserverMap;
}

inline void AxisDistanceManager::AddDelayCalcTable(ActorID id) {
	m_DelayCalcMoveList.insert(id);//这个角色将再下一帧被重计算
}
inline bool AxisDistanceManager::RegisterBody(BodyData* actor) {
	ActorID actorID = actor->ID();
	if (m_ViewObjMap.count(actorID)) return false;//对象已经注册  
	m_ViewObjMap[actorID] = new ViewRange(actor);//注册进入即可
	AddDelayCalcTable(actorID);//在下一帧重新计算角色信息
	return true;
}

inline void AxisDistanceManager::UnregisterBody(ActorID id)
{
	if (!m_ViewObjMap.count(id)) return;//未注册过
	ViewRange* rangeObj = m_ViewObjMap[id];//获取到对象 
	RemoveViewAABB( rangeObj);
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


inline void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
//重新计算角色距离
inline void AxisDistanceManager::Update() {
	ActorsMove();//更新所有角色的视距,包括碰撞AABB  
	CalcViewObj();//计算角色可以观察到的视图对象  
}
  
inline AxisDistanceManager& AxisDistanceManager::Instance() {
	static AxisDistanceManager Instance;
	return Instance;
}
inline ViewRange* AxisDistanceManager::GetViewRange(ActorID actorID)
{
	return m_ViewObjMap[actorID];
}