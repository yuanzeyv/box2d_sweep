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
	for (int type = 0; type < PointAxisType::MAX_AXIS_TYPE; type++) {
		for (int i = 0;i < m_RecordArray[type].size();i++) {
			m_InPosActorsGenerate.BackObj(m_RecordArray[type][i]);
		}
		for (auto item = m_AxisActors[type].begin();item != m_AxisActors[type].end();item++) {
			m_b2VecIdleGenerate.BackObj(item->first);
		}
		m_AxisActors[type].clear();
		m_RecordArray[type].resize(0);
	}
}

bool AxisMap::AddtionViewRange(ViewRange* actor, b2AABB& aabb)
{
	b2Vec2* point[PointAxisType::MAX_AXIS_TYPE];
	point[PointAxisType::LEFT_BUTTOM] = &aabb.lowerBound;
	point[PointAxisType::RIGHT_TOP] = &aabb.upperBound;
	ActorID actorId = actor->GetActor()->ID();
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++)
	{
		if (m_AxisActors[i].count(point[i]) == 0) {
			auto b2Vec2Key = m_b2VecIdleGenerate.RequireObj();
			memcpy(b2Vec2Key, point[i], sizeof(b2Vec2));
			m_AxisActors[i][b2Vec2Key] = skiplist_new();
		}
		auto jumpList = m_AxisActors[i][point[i]];
		if (!skiplist_search(jumpList, actorId)){
			skiplist_insert(jumpList, actorId,(int64_t)actor);
		} 
	}
	return true;
}
void AxisMap::DeleteViewRange(ViewRange* actor, b2AABB& aabb)
{
	b2Vec2* point[PointAxisType::MAX_AXIS_TYPE];
	point[PointAxisType::LEFT_BUTTOM] = &aabb.lowerBound;
	point[PointAxisType::RIGHT_TOP] = &aabb.upperBound;
	ActorID actorId = actor->GetActor()->ID();
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++)
	{
		if (m_AxisActors[i].count(point[i]) == 0)
			continue;
		auto jumpList = m_AxisActors[i][point[i]];
		skiplist_remove(jumpList, actorId);
		if (jumpList->count == 0) {
			delete jumpList;
			m_b2VecIdleGenerate.BackObj(m_AxisActors[i].find(point[i])->first);
			m_AxisActors[i].erase(point[i]);
		} 
	}
}
//准备计算了,初始化当前的数据信息
void AxisMap::ReadyInitActorData()
{ 
	for (int i = 0; i < PointAxisType::MAX_AXIS_TYPE; i++) {
		auto& actorMap = m_AxisActors[i];
		size_t size = actorMap.size();//获取到当前的大小
		size_t nowSize = m_RecordArray[i].size();//获取到当前的大小
		if (size != nowSize) { 
			if ((size - nowSize) > 0) {//需要添加的情况
				m_RecordArray[i].resize(size);
				InPosActors** actorData = m_RecordArray[i].data();
				for (int index = nowSize;index < size;index++)
					actorData[index] = m_InPosActorsGenerate.RequireObj();
			} else {
				InPosActors** actorData = m_RecordArray[i].data();
				for (int index = nowSize;index < size;index++)
					m_InPosActorsGenerate.BackObj(actorData[index]);
				m_RecordArray[i].resize(size);
			}
		}
		int index = 0; 
		InPosActors** actorData = m_RecordArray[i].data();//重新设置数组的大小
		auto itorEnd = actorMap.end(); 
		for (auto actorSet = actorMap.begin(); actorSet != itorEnd; actorSet++,index++) {
			actorData[index]->m_Position.x = actorSet->first->x;
			actorData[index]->m_Position.y = actorSet->first->y;
			actorData[index]->m_SkipList = actorSet->second; 
		}
	}
}//多个有序数组排序
struct node {
	sk_link* m_Point;  //  元素值
	ViewRange* m_ViewRange;  //  元素值
	int k;   //元素所在数组
	int i;  // 元素所在数组下标
};
struct compare
{
	bool operator()(node& node1, node& node2)
	{
		if (node1.i == INT_MAX || node2.i == INT_MAX)
			return node1.i > node2.i;
		return node1.m_ViewRange->GetActor()->ID() > node2.m_ViewRange->GetActor()->ID();//这是小根堆
	}
};
void mul_sort(std::vector<InPosActors*>& nums, vector<ViewRange*>& res) {
	int size = 0;
	for (auto item = nums.begin(); item != nums.end(); item++)
		size += (*item)->m_SkipList->count;
	res.resize(size);

	int k = nums.size();
	priority_queue<node, vector<node>, compare> que;
	for (int i = 0; i < k; i++) { 
		struct sk_link* pos = nums[i]->m_SkipList->head[0].next; 
		struct skipnode* node = list_entry(pos, struct skipnode, link[0]);
		ViewRange* range = (ViewRange*)node->value;
		que.push({ pos,range,i,0 });
	} 
	for (int j = 0; j < size; j++) {
		node currt = que.top();
		que.pop();
		res[j] = currt.m_ViewRange;
		node next;
		next.i = currt.i + 1;
		next.k = currt.k;
		if (next.i >= nums[currt.k]->m_SkipList->count) {
			next.i = INT_MAX; 
		}
		else {
			next.m_Point = currt.m_Point->next;
			next.m_ViewRange = (ViewRange*)list_entry(next.m_Point, struct skipnode, link[0])->value;
		}
		que.push(next);
	}
}
static bool ViewRangeIDCompare(ViewRange* node1, ViewRange* node2)
{ 
	return node1->GetActor()->ID() < node2->GetActor()->ID();
}
// 获取到范围内的所有合适的角色
int AxisMap::GetRangeActors(std::vector<ViewRange*>& outData, const b2AABB& range)
{ 

	std::vector<InPosActors*> lowPointArr;//低点匹配的点
	std::vector<InPosActors*> upPointArr;//高点匹配的点
	std::vector<ViewRange*> lowRangeArr;//低点匹配的点
	std::vector<ViewRange*> upRangeArr;//高点匹配的点
	outData.resize(0);
	InPosActors posActor;
	posActor.m_Position = range.upperBound;
	int index = 0;
	auto lowPointItor = std::upper_bound(m_RecordArray[PointAxisType::LEFT_BUTTOM].begin(), m_RecordArray[PointAxisType::LEFT_BUTTOM].end(), &posActor, InPosActors()); //先判断了X,在判断一遍Y
	lowPointArr.resize(lowPointItor - m_RecordArray[PointAxisType::LEFT_BUTTOM].begin());
	InPosActors** posData = lowPointArr.data();
	for (auto item = m_RecordArray[PointAxisType::LEFT_BUTTOM].begin();item != lowPointItor;item++)
		if (((*item)->m_Position.y - range.upperBound.y) <= 0)
			posData[index++] = *item;
	lowPointArr.resize(index);
	mul_sort(lowPointArr, lowRangeArr);
	 
	index = 0;
	posActor.m_Position = range.lowerBound;
	auto upPointItor = std::lower_bound(m_RecordArray[PointAxisType::RIGHT_TOP].begin(), m_RecordArray[PointAxisType::RIGHT_TOP].end(), &posActor, InPosActors());//++  
	upPointArr.resize(m_RecordArray[PointAxisType::RIGHT_TOP].end() - upPointItor );
	if (upPointArr.size() > 0) {
		posData = upPointArr.data();
		for (auto item = upPointItor;item != m_RecordArray[PointAxisType::RIGHT_TOP].end();item++) {
			if ((range.lowerBound.y - (*item)->m_Position.y) <= 0)
				posData[index++] = *item;
		}
		upPointArr.resize(index);
		mul_sort(upPointArr, upRangeArr);
		outData.resize(min(upRangeArr.size(), lowRangeArr.size())); 
		outData.resize(set_intersection(lowRangeArr.begin(), lowRangeArr.end(), upRangeArr.begin(), upRangeArr.end(), outData.begin(), ViewRangeIDCompare) - outData.begin());
	} 
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
		//开始删除角色上一帧的信息
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


	auto t1 = std::chrono::high_resolution_clock::now();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto moveCost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "     加数据耗时:" << moveCost << std::endl;

	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) { 
		ActorID actorID = (*item)->GetActor()->ID();
		actoViewange = *item; 
		m_AxisBodyMap.GetRangeActors(actoViewange->GetObserverArray(), actoViewange->GetViewAABB()); 
	}
	m_CalcMoveList.clear();
} 