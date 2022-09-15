#pragma once
#include "Manager/Base/BodyData.h" 
#include <map>
#include <list>      
#include <unordered_map>
#include <unordered_set>
#include <chrono> 
#include <set>
#include <algorithm>
#include <typeinfo>
#include <algorithm>
#include "Define.h"  
#include "TemplateBoard.h"  
#include "Library/JumpList/JumpList.h"
class ViewRange;
enum PointType {
	BODY_TYPE = 0,//刚体类型
	VIEW_TYPE = 1,//视图类型
	MAX_POINT_TYPE = 2,
};  
struct XAxisComapare {
	bool operator()(const float& xAxis1, const float& xAxis2) const {
		return xAxis1 < xAxis2; 
	}
};
class InPosActors {
public:
	InPosActors() {}
	bool operator()(const InPosActors* id1, const InPosActors* id2);
public:
	float m_PositionX;
	struct skiplist* m_SkipList;
};
struct ViewObserverState {
	ViewRange* m_ViewRange;
	bool m_IsVisible; 
};

class AxisMap{
private:
	std::map<float, struct skiplist*, XAxisComapare> m_XAxisActors[PointType::MAX_POINT_TYPE];//以每个AABB的左边点的X进行排序.
	std::vector<InPosActors*> m_RecordArray[PointType::MAX_POINT_TYPE];//为了防止重复遍历,浪费性能. 在每次准备寻找时,需要将所有map的数据,拷贝到数组中去,以加快性能
	AutomaticGenerator<b2Vec2> m_b2VecIdleGenerate;//自动的生成视图对象管理的对象 
	AutomaticGenerator<InPosActors> m_InPosActorsGenerate;//自动的生成视图对象管理的对象 
public:
	void AddtionViewRange(PointType type,ViewRange* actor, b2AABB& aabb);//添加一个点位
	void DeleteViewRange(PointType type, ViewRange* actor, b2AABB& aabb);//删除一个点位  
	void ReadyInitActorData();//初始化点位 
	 
	int SetRangeActors(PointType type,ViewRange* actor, std::vector<ViewObserverState*>& outData, const b2AABB& range);//寻找相交 的AABB  

	AxisMap():m_InPosActorsGenerate(200) {}
	~AxisMap();
}; 

class AxisDistanceManager;
class ViewRange{
public:
	friend class AxisMap;
	friend class AxisDistanceManager;
public:
	ViewRange(BodyData* bodyData,b2Vec2 range = b2Vec2(0.5, 0.5));//视图的初始化
	BodyData* GetActor();//当前的视图对象
	const b2Vec2& GetViewRange(); //获取到当前的观察范围
	const b2Vec2& GetBodyPos();//获取到当前玩家的点位 
	void GetObserverArray(std::vector<ViewRange*>& outData);//获取到所有可见角色 
	std::vector<ViewObserverState*>& GetBeObserverArray();//获取到所有可见角色 
	b2AABB& GetBodyAABB();//获取到当前角色的刚体AABB   
	b2AABB& GetViewAABB();//获取到当前角色的刚体AABB  

	bool IsRefreshActorView();//计算视差,大于多少才会重计算  
	void RecalcBodyPosition();//重新获取到角色上一次未刷新的位置

	void RecalcBodyAABB();//重新计算视口 
	void RecalcViewAABB();//重新计算视口 
	
	void ClearExtraObserverArr();//删除所有额外的观察对象
	void ChangeObserverStatus(ViewRange* beObserverActor, bool status);
	void ShiftObserverPoint();//转换观察指针

	void ClearExtraBeObserverArr();//删除所有额外的观察对象
	void ChangeBeObserverStatus(ViewRange* beObserverActor, bool status);
	void ShiftBeObserverPoint();//转换观察指针

	void RecalcRefreshCondtion();//重计算刷新条件    

	void RecycleStatusObj(std::vector<ViewObserverState*>* obj)
	{
		for (int i = 0; i < obj->size();i++)
			m_ViewObserverStateGenerate.BackObj(obj->at(i));
		obj->clear();
	}
	void RecycleStatusObj(std::unordered_map<ActorID, ViewObserverState*>& obj)
	{
		for(auto item = obj.begin();item != obj.end();item++)
			m_ViewObserverStateGenerate.BackObj(item->second); 
		obj.clear();
	}
	~ViewRange(){
		RecycleStatusObj(m_ObserverPoint);
		RecycleStatusObj(m_FrontObserverPoint);
		RecycleStatusObj(m_FrontBeObserverPoint);
		RecycleStatusObj(m_BeObserverPoint);
		RecycleStatusObj(m_ExtraBeObserverArr);
		RecycleStatusObj(m_ExtraObserverArr);
	}
private:
	BodyData* m_Actor;//当前的角色 
	b2Vec2 m_BodyPos;//上一次的角色位置

	std::vector<ViewObserverState*>* m_FrontObserverPoint;//第一个原始代表上一帧可以看到的
	std::vector<ViewObserverState*>* m_ObserverPoint;//第二个代表本帧
	std::vector<ViewObserverState*> m_ObserverArr[2];//可以看到的角色列表.
	std::unordered_map<ActorID,ViewObserverState*> m_ExtraObserverArr;//当值被别人观察到的时候,不好直接插入数组,所有使用这个方法

	std::vector<ViewObserverState*>* m_FrontBeObserverPoint;//第一个原始代表上一帧可以看到的
	std::vector<ViewObserverState*>* m_BeObserverPoint;//第二个代表本帧
	std::vector<ViewObserverState*> m_BeObserverArr[2];//可以看到的角色列表.
	std::unordered_map<ActorID, ViewObserverState*> m_ExtraBeObserverArr;//可以看到的角色列表    

	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围  

	b2AABB m_AABB[PointType::MAX_POINT_TYPE];//记录角色上一次添加时的AABB      
	b2Vec2 m_OffsetCondtion;//偏移极限 

	AutomaticGenerator<ViewObserverState> m_ViewObserverStateGenerate;//自动的生成视图对象管理的对象 
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
	//获取到单元
	ViewRange* GetViewRange(ActorID actorID);
	//重计算
	void Update();//重新计算角色距离
	void ActorsMove();
	void CalcViewObj();//重计算 
	void CalcObserver(std::vector<ViewRange*>& moveRangeArr);//重计算 
	void CalcBeObserver(std::vector<ViewRange*>& moveRangeArr);//重计算 
	static AxisDistanceManager& Instance();
private:
	AxisDistanceManager() { m_AxisBodyMap = new AxisMap; }
	~AxisDistanceManager() { delete m_AxisBodyMap; }
private:
	//当前所有的距离对象
	AxisMap* m_AxisBodyMap;//其中有X坐标列和Y坐标列 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//角色对应的范围信息 

	std::map<ActorID, ViewRange*> m_DelayCalcMoveList;//角色对应的范围信息
	std::map<ActorID,ViewRange*> m_CalcMoveList;//应该被计算的列表
}; 
 
inline const b2Vec2& ViewRange::GetViewRange() //获取到当前的视图范围
{ 
	return m_ObserverRange;
}
inline void ViewRange::ShiftObserverPoint() {
	auto tempPoint = m_FrontObserverPoint;
	m_FrontObserverPoint = m_ObserverPoint;
	m_ObserverPoint = tempPoint; //置换一下本帧信息  
}
inline void ViewRange::ShiftBeObserverPoint() {
	auto tempPoint = m_FrontBeObserverPoint;
	m_FrontBeObserverPoint = m_BeObserverPoint;
	m_BeObserverPoint = tempPoint; //置换一下本帧信息  
} 
inline const b2Vec2& ViewRange::GetBodyPos() {//获取到当前玩家的点位

	return m_BodyPos; 
} 
inline b2AABB& ViewRange::GetBodyAABB()//获取到当前角色的刚体AABB 
{
	return m_AABB[PointType::BODY_TYPE];
}
inline b2AABB& ViewRange::GetViewAABB()//获取到当前角色的刚体AABB 
{
	return m_AABB[PointType::VIEW_TYPE];//获取到某一类型的AABB盒子
}
 
inline void ViewRange::RecalcViewAABB()//重新计算视口
{
	m_AABB[PointType::VIEW_TYPE].lowerBound.Set(m_BodyPos.x - m_ObserverRange.x, m_BodyPos.y - m_ObserverRange.x);//重新计算视图
	m_AABB[PointType::VIEW_TYPE].upperBound.Set(m_BodyPos.x + m_ObserverRange.y, m_BodyPos.y + m_ObserverRange.y);
}
inline void ViewRange::RecalcBodyAABB()//重新计算视口
{ 
	m_Actor->CalcBodyAABB(); 
	memcpy(&m_AABB[PointType::BODY_TYPE], &m_Actor->GetAABB(), sizeof(b2AABB));//拷贝数据
	RecalcRefreshCondtion(); 
} 

inline void ViewRange::RecalcBodyPosition()
{ 
	m_BodyPos = m_Actor->GetBody()->GetPosition();
}
inline void ViewRange::RecalcRefreshCondtion()//重计算刷新条件
{	//角色移动了相对于角色当前百分之10的位置的话
	m_OffsetCondtion.x = (m_AABB[PointType::BODY_TYPE].upperBound.x - m_AABB[PointType::BODY_TYPE].lowerBound.x) * 0.2f;
	m_OffsetCondtion.y = (m_AABB[PointType::BODY_TYPE].upperBound.y - m_AABB[PointType::BODY_TYPE].lowerBound.y) * 0.2f;
}
inline BodyData* ViewRange::GetActor() {//获取到当前的角色
	return m_Actor;
}
  
inline bool ViewRange::IsRefreshActorView()//计算视差,大于多少才会重计算
{
	const b2Vec2& bodyPos = m_Actor->GetBody()->GetPosition();//获取到当前的位置 
	float xMinus = std::abs(bodyPos.x - m_BodyPos.x);
	float yMinus = std::abs(bodyPos.y - m_BodyPos.y);
	if (xMinus > m_OffsetCondtion.x || yMinus > m_OffsetCondtion.y)
		return true;
	return false;
}
inline void ViewRange::GetObserverArray(std::vector<ViewRange*>& outData)//获取到所有可见角色
{ 
	for (int i = 0;i < m_BeObserverPoint->size();i++){
		if (m_BeObserverPoint->at(i)->m_IsVisible == 0)
			outData.push_back(m_BeObserverPoint->at(i)->m_ViewRange); 
	} 
	int index = m_BeObserverPoint->size();
	for (auto item = m_ExtraObserverArr.begin();item != m_ExtraObserverArr.end();item++, index++) {
		if (item->second->m_IsVisible == 0)
			outData.push_back(item->second->m_ViewRange); 
	} 
}
inline std::vector<ViewObserverState*>& ViewRange::GetBeObserverArray()//获取到所有可见角色
{
	return *this->m_BeObserverPoint;
}

inline void AxisDistanceManager::AddDelayCalcTable(ActorID id) { 
	if (m_DelayCalcMoveList.count(id))
		return;
	if (m_ViewObjMap.count(id) == 0)
		return; 
	m_DelayCalcMoveList[id] = m_ViewObjMap[id];
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
	m_AxisBodyMap->DeleteViewRange(PointType::BODY_TYPE, rangeObj, rangeObj->GetBodyAABB());
	m_AxisBodyMap->DeleteViewRange(PointType::VIEW_TYPE, rangeObj, rangeObj->GetViewAABB());
	delete rangeObj;
	m_ViewObjMap.erase(id);
}


inline void AxisDistanceManager::UnregisterBody(BodyData* actor)
{
	UnregisterBody(actor->ID());
}
//重新计算角色距离
inline void AxisDistanceManager::Update() {
	//针对所有移动过的角色重新计算他们的可视范围内的所有 

	int64_t moveCost;
	int64_t calcCost;
	auto t1 = std::chrono::high_resolution_clock::now();
	ActorsMove();//更新所有角色的视距,包括碰撞AABB  
	auto t2 = std::chrono::high_resolution_clock::now();
	moveCost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "待计算的单位有:" << m_CalcMoveList.size() << std::endl;
	t1 = std::chrono::high_resolution_clock::now();
	CalcViewObj();//计算角色可以观察到的视图对象  
	t2 = std::chrono::high_resolution_clock::now();
	calcCost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count(); 
	std::cout << "移动耗时" << moveCost << " 计算耗时:" << calcCost << "总耗时:" << moveCost + calcCost << std::endl;
}
  
inline AxisDistanceManager& AxisDistanceManager::Instance() {
	static AxisDistanceManager Instance;
	return Instance;
}
inline ViewRange* AxisDistanceManager::GetViewRange(ActorID actorID)
{
	return m_ViewObjMap[actorID];
}

inline bool InPosActors::operator()(const InPosActors* id1, const InPosActors* id2) {
	return id1->m_PositionX < id2->m_PositionX;
}