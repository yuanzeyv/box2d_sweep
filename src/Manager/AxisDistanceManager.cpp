#include "Manager/AxisDistanceManager.h" 
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999){
} 
 
 
void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//首先移动所有的角色
	bool isDelete = false;//当前是否仅仅计算  
	auto iterEnd = m_DelayCalcMoveList.end();//防止重复获取到end
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { //循环遍历到所有的移动的角色
		ActorID actorID = item->first; //获取到角色ID
		actorView = item->second;//获取到角色的视图数据
		if(actorView->IsRefreshActorView() == 0)//移动距离过浅
			continue;
		actorView->RecalcBodyPosition();//重计算角色的位置  
		//开始删除角色上一帧的信息
		m_AxisBodyMap.DeleteViewRange(actorID, actorView->GetBodyAABB()); 
		actorView->RecalcBodyAABB();
		m_AxisBodyMap.AddtionViewRange(actorID, actorView->GetBodyAABB());
		m_CalcMoveList.push_back(item->second);
	}  
	m_DelayCalcMoveList.clear(); 
}
  
//寻找某一个轴的某一范围内的所有单元信息  
void AxisDistanceManager::InquiryAxisPoints(unordered_set<ActorID>& outData,const b2AABB& viewAABB)//偏移单元代表当前要查询的息(必须确保当前被查询点存在于表内)
{  
}
//重计算
void AxisDistanceManager::CalcViewObj()
{ 
	//重新计算自己所见所得
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) {
		//首先计算当前的操作 
		ActorID actorID = (*item)->GetActor()->ID();
		actoViewange = *item;
		actoViewange->ClearObserverTable();//清除,然后重新计算
		unordered_set<ActorID> XSet;
		const b2AABB& viewAABB = actoViewange->GetViewAABB();
		InquiryAxisPoints(XSet, viewAABB);//获取到X轴的列表  
		for (auto relevanceItem = XSet.begin(); relevanceItem != XSet.end(); relevanceItem++) {
			auto obseverItem = m_ViewObjMap[*relevanceItem]; 
			actoViewange->EnterView(obseverItem); 
		}
	}
	m_CalcMoveList.clear();
} 