#include "Manager/AxisDistanceManager.h" 

#include<queue>
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999){
} 

AxisMap::~AxisMap() {
		for (int i = 0;i < m_RecordArray.size();i++) {
			m_InPosActorsGenerate.BackObj(m_RecordArray[i]);
		} 
		m_XAxisActors.clear();
		m_RecordArray.resize(0); 
}

void AxisMap::AddtionViewRange(ViewRange* actor, b2AABB& aabb)
{
	float lowX = aabb.lowerBound.x;//保存低点的X 
	ActorID actorId = actor->GetActor()->ID();
	if (m_XAxisActors.count(lowX) == 0)
		m_XAxisActors[lowX] = skiplist_new();
	auto jumpList = m_XAxisActors[lowX];
	if (!skiplist_search(jumpList, actorId)){//插入
		skiplist_insert(jumpList, actorId,(int64_t)actor);
	}  
}
void AxisMap::DeleteViewRange(ViewRange* actor, b2AABB& aabb)
{
	float lowX = aabb.lowerBound.x;//保存低点的X   
	ActorID actorId = actor->GetActor()->ID(); 
	if (m_XAxisActors.count(lowX) == 0)
		return;
	auto jumpList = m_XAxisActors[lowX];
	skiplist_remove(jumpList, actorId);
	if (jumpList->count == 0) {
		skiplist_delete(jumpList);
		m_XAxisActors.erase(lowX);
	}  
}
//准备计算了,初始化当前的数据信息
void AxisMap::ReadyInitActorData()
{
	int index = 0;
	size_t size = m_XAxisActors.size();//获取到当前的大小
	size_t nowSize = m_RecordArray.size();//获取到当前的大小
	if (size != nowSize) { 
		if ((size - nowSize) > 0) {//需要添加的情况
			m_RecordArray.resize(size);
			InPosActors** actorData = m_RecordArray.data();
			for (int index = nowSize;index < size;index++)
				actorData[index] = m_InPosActorsGenerate.RequireObj();
		} else {
			InPosActors** actorData = m_RecordArray.data();
			for (int index = nowSize;index < size;index++)
				m_InPosActorsGenerate.BackObj(actorData[index]);
			m_RecordArray.resize(size);
		}
	}
	InPosActors** actorData = m_RecordArray.data();//重新设置数组的大小
	auto itorEnd = m_XAxisActors.end();
	for (auto actorSet = m_XAxisActors.begin(); actorSet != itorEnd; actorSet++,index++) {
		actorData[index]->m_PositionX = actorSet->first;
		actorData[index]->m_SkipList = actorSet->second; 
	} 
} 
static bool ViewRangeIDCompare(ViewRange* node1, ViewRange* node2)
{ 
	return node1->GetActor()->ID() < node2->GetActor()->ID();
}
// 获取到范围内的所有合适的角色
int AxisMap::GetRangeActors(std::vector<ViewRange*>& outData, const b2AABB& range)
{ 
	std::vector<ViewRange*> lowRangeArr;//低点匹配的点 
	outData.resize(0); 
	InPosActors posActor; 
	posActor.m_PositionX = range.upperBound.x;
	int index = 0;
	auto lowPointItorSize = std::lower_bound(m_RecordArray.begin(), m_RecordArray.end(), &posActor, InPosActors()) - m_RecordArray.begin(); //先判断了X,在判断一遍Y
	InPosActors** recordData = m_RecordArray.data();
	for (int i = 0; i < lowPointItorSize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList;
		lowRangeArr.resize(skipList->count);
		GetJumpListValue(skipList, lowRangeArr);//获取到所有符合描述的点  
		ViewRange** viewRangeArr = lowRangeArr.data();
		ViewRange* viewRangeTemp = NULL;
		for (int i = 0; i < lowRangeArr.size(); i++) {
			viewRangeTemp = viewRangeArr[i];
			b2AABB& AABB = viewRangeTemp->GetBodyAABB();
			//判断y点
			if (AABB.lowerBound.x > range.upperBound.x || AABB.lowerBound.y > range.upperBound.y || range.lowerBound.x > AABB.upperBound.x || range.lowerBound.y > AABB.upperBound.y)
				continue;
			outData.push_back(viewRangeTemp);
		}
	}
	//auto t1 = std::chrono::high_resolution_clock::now(); 
	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto sort1Cost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << "     第一次排序耗时:" << sort1Cost << std::endl; 
	sort(outData.begin(), outData.end(), ViewRangeIDCompare); //排序之后,便可用了
	return outData.size();
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
		m_AxisBodyMap.DeleteViewRange(actorView, actorView->GetBodyAABB());
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		m_AxisBodyMap.AddtionViewRange(actorView, actorView->GetBodyAABB());
		m_CalcMoveList.push_back(item->second);
	}  
	m_DelayCalcMoveList.clear(); 
} 
 
//重计算
void AxisDistanceManager::CalcViewObj()
{ 
	//准备计算数据
	m_AxisBodyMap.ReadyInitActorData(); 
	//重新计算自己所见所得
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) { 
		actoViewange = *item;
		m_AxisBodyMap.GetRangeActors(actoViewange->GetObserverArray(), actoViewange->GetViewAABB());
	}
	m_CalcMoveList.clear();   
} 