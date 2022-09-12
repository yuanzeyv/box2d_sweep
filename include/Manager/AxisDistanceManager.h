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
enum PointType {
	BODY_TYPE = 0,//刚体类型
	VIEW_TYPE = 1,//视图类型
	MAX_POINT_TYPE = 2,
}; 
enum PointAxisType {
	LEFT_BUTTOM = 0,
	RIGHT_TOP   = 1,
	MAX_AXIS_TYPE,
};
struct b2VecCompare {
	bool operator()(const b2Vec2*& k1, const b2Vec2*& k2) const {
		if (k1->x != k2->x)
			return k1->x < k2->x;
		return k1->y < k2->y;
	}
};  
class InPosActors{
public:
	static inline bool InPosActorsCompare(InPosActors& id1, InPosActors& id2) {
		if (id1.m_Position.x != id2.m_Position.x)
			return id1.m_Position.x < id2.m_Position.x; 
		return id1.m_Position.y < id2.m_Position.y;
	}
public:
	InPosActors() :m_Position() {
	}
	b2Vec2 m_Position;
	std::set<ActorID> m_ActorsSet;
};

class AxisMap{
private: 
	std::map<b2Vec2*,std::unordered_set<ActorID>, b2VecCompare> m_AxisActors[PointAxisType::MAX_AXIS_TYPE];//记录一组以点位保存的无序的视图对象

	std::vector<InPosActors*> m_RecordArray[PointAxisType::MAX_AXIS_TYPE];//为了防止重复遍历,浪费性能. 在每次准备寻找时,需要将所有map的数据,拷贝到数组中去,以加快性能
	AutomaticGenerator<b2Vec2> m_b2VecIdleGenerate;//自动的生成视图对象管理的对象 
public:
	inline bool AddtionViewRange(ActorID actorID, b2AABB& aabb)
	{
		b2Vec2* point[PointAxisType::MAX_AXIS_TYPE];
		point[PointAxisType::LEFT_BUTTOM] = &aabb.lowerBound;
		point[PointAxisType::RIGHT_TOP] = &aabb.upperBound; 
		for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++)
		{
			if (m_AxisActors[i].count(point[i]) == 0) {
				auto b2Vec2Key = m_b2VecIdleGenerate.RequireObj();
				m_AxisActors[i][b2Vec2Key];
			}
			auto actorSet = m_AxisActors[i][point[i]];
			if (actorSet.count(actorID) == 0)
				actorSet.insert(actorID);
		}
	}
	inline void DeleteViewRange(ActorID actorID, b2AABB& aabb)
	{
		b2Vec2* point[PointAxisType::MAX_AXIS_TYPE];
		point[PointAxisType::LEFT_BUTTOM] = &aabb.lowerBound;
		point[PointAxisType::RIGHT_TOP] = &aabb.upperBound;
		for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++)
		{
			if (m_AxisActors[i].count(point[i]) == 0)
				continue;
			auto actorSet = m_AxisActors[i][point[i]];
			if (actorSet.count(actorID))
				m_AxisActors[i][point[i]].erase(actorID);
			if (actorSet.size() == 0) {
				m_b2VecIdleGenerate.BackObj(m_AxisActors[i].find(point[i])->first);
				m_AxisActors[i].erase(point[i]);
			}
		}
	} 
	//准备计算了,初始化当前的数据信息
	inline void ReadyInitActorData()
	{
		//初始化数据,由于使用set所以默认是有序的
		for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++) {
			auto& recordArray = m_RecordArray[i];
			auto& actorSet = m_AxisActors[i];
			recordArray.resize(actorSet.size());//重新设置数组的大小
			InPosActors** actorData = recordArray.data();//重新设置数组的大小
			for (auto item = actorSet.begin(); item != actorSet.end(); item++)
				actorData[i] = *item;
		}
	}
	// 获取到范围内的所有合适的角色
	inline int GetRangeActors(std::vector<ActorID>& outData, b2AABB& range)
	{ 
		std::vector<InPosActors*> compareArr;//当前的数据信息
		//首先通过2分查找法,快速找到符合要求的那个点位
		//排除最小点大于 range 的最大点的
		auto lowPointItor = std::lower_bound(m_RecordArray[PointAxisType::LEFT_BUTTOM].begin(), m_RecordArray[PointAxisType::LEFT_BUTTOM].begin(), range.upperBound); //++
		auto upPointItor = std::upper_bound(m_RecordArray[PointAxisType::RIGHT_TOP].begin(), m_RecordArray[PointAxisType::RIGHT_TOP].begin(), range.lowerBound);//-- 如果有相同的,直接开始迭代,否则减减后开始迭代
		std::copy(lowPointItor, m_RecordArray[PointAxisType::LEFT_BUTTOM].end(), compareArr.begin());//从头开始赋值
		auto midItor = compareArr.end();//获取到前的中间值
		std::copy(m_RecordArray[PointAxisType::RIGHT_TOP].begin(), upPointItor, compareArr.begin());//从头开始赋值
		std::inplace_merge(compareArr.begin(), midItor, compareArr.end()); 
		auto itorEnd = compareArr.end();
		for (auto item = compareArr.begin(); item != itorEnd; item++) {
			auto bestItor = std::lower_bound(outData.begin(), outData.end(),*(*item)->m_ActorsSet.begin());//找到元数组最合适的插入位置
			std::copy((*item)->m_ActorsSet.begin(), (*item)->m_ActorsSet.end(), outData.end());//合并
			std::inplace_merge(bestItor, outData.begin() + (*item)->m_ActorsSet.size(), outData.end()); 
		}
	}                                                                                                                                                                                    
}; 

class ViewRange{
public:
	ViewRange(BodyData* bodyData,b2Vec2 range = b2Vec2(0.5, 0.5));//视图的初始化
	BodyData* GetActor();//当前的视图对象
	const b2Vec2& GetViewRange(); //获取到当前的观察范围
	const b2Vec2& GetBodyPos();//获取到当前玩家的点位 
	const std::vector<ActorID>& GetObserverArray();//获取到所有可见角色
	const std::unordered_map<ActorID, ViewRange*>& GetBeObseverView(ViewRange* actor);//某一个角色进入到了被观察的视图
	b2AABB& GetBodyAABB() const;//获取到当前角色的刚体AABB   
	const b2AABB& GetViewAABB() const;//获取到当前角色的刚体AABB 
	const b2AABB& GetAABB(PointType type) const;//获取到当前角色的刚体AABB 

	void RefreshObserver(std::vector<ActorID>& actorArr);//每一帧只要进入的,就会直接刷新 

	bool InObserverContain(ViewRange* actor); //计算制定AABB是包含自己 

	int IsRefreshActorView();//计算视差,大于多少才会重计算  
	void RecalcBodyPosition();//重新获取到角色上一次未刷新的位置

	void RecalcBodyAABB();//重新计算视口 
	void RecalcViewAABB();//重新计算视口
	void RecalcAABB(PointType type);//重新计算视口

	void RecalcRefreshCondtion();//重计算刷新条件    

	void GetObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor);//寻找自己视口的某一个对象
	void GetBeObserverActor(std::vector<ViewRange*>::iterator& itor, ViewRange* actor);//寻找自己视口的某一个对象

	static bool Compare(ViewRange*& v1, ViewRange*& v2);
	~ViewRange() {}
private:
	BodyData* m_Actor;//当前的角色 
	b2Vec2 m_BodyPos;//上一次的角色位置

	std::vector<ActorID> m_ObserverArr;//可以看到的角色列表    

	b2Vec2 m_ObserverRange;//当前角色可以观察到的范围  

	b2AABB m_AABB[PointType::MAX_POINT_TYPE];//记录角色上一次添加时的AABB      
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
	//当前所有的距离对象
	AxisMap m_AxisBodyMap;//其中有X坐标列和Y坐标列 
	std::unordered_map<ActorID, ViewRange*> m_ViewObjMap;//角色对应的范围信息 

	std::unordered_map<ActorID, ViewRange*> m_DelayCalcMoveList;//角色对应的范围信息
	std::vector<ViewRange*> m_CalcMoveList;//应该被计算的列表
}; 

inline void ViewRange::RefreshObserver(std::vector<ActorID>& actorArr)
{ 
	m_ObserverArr.resize(actorArr.size());
	memcpy(m_ObserverArr.data(), actorArr.data(),sizeof(actorArr.size() * sizeof(ActorID)));
	AxisDistanceManager& manager = AxisDistanceManager::Instance();  
} 
inline const b2Vec2& ViewRange::GetViewRange() //获取到当前的视图范围
{ 
	return m_ObserverRange;
}
inline const b2Vec2& ViewRange::GetBodyPos() {//获取到当前玩家的点位

	return m_BodyPos; 
} 
inline b2AABB& ViewRange::GetBodyAABB() const//获取到当前角色的刚体AABB 
{
	return m_AABB[PointType::BODY_TYPE];
}
inline const b2AABB& ViewRange::GetViewAABB() const//获取到当前角色的刚体AABB 
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
inline void ViewRange::RecalcAABB(PointType type)//重新计算视口
{
	if (type == PointType::BODY_TYPE)
		RecalcBodyAABB();
	else if (type == PointType::VIEW_TYPE)
		RecalcViewAABB();
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

inline bool ViewRange::InObserverContain(ViewRange* actor) {//移动的角色询问当前是否还在监听范围内  
	auto startItor = std::lower_bound(m_ObserverArr.begin(), m_ObserverArr.end(),actor->GetActor()->ID());
	return startItor != m_ObserverArr.end() && *startItor == actor->GetActor()->ID(); 
}
inline const b2AABB& ViewRange::GetAABB(PointType type) const//获取到当前角色的刚体AABB 
{
	return m_AABB[type];
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
inline const std::vector<ActorID>& ViewRange::GetObserverArray()//获取到所有可见角色
{
	return this->m_ObserverArr;
}
  

inline void AxisDistanceManager::AddDelayCalcTable(ActorID id) { 
	if (m_DelayCalcMoveList.count(id))
		return;
	if (m_ViewObjMap.count(id) == 0)
		return; 
	m_DelayCalcMoveList[id] = m_ViewObjMap[id];//这个角色将再下一帧被重计算
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
	RemoveAABB(PointType::BODY_TYPE,rangeObj);
	RemoveAABB(PointType::BODY_TYPE, rangeObj);
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