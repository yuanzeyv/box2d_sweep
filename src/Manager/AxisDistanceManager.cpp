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
	float lowX = aabb.lowerBound.x;//����͵��X 
	ActorID actorId = actor->GetActor()->ID();
	if (m_XAxisActors.count(lowX) == 0)
		m_XAxisActors[lowX] = skiplist_new();
	auto jumpList = m_XAxisActors[lowX];
	if (!skiplist_search(jumpList, actorId)){//����
		skiplist_insert(jumpList, actorId,(int64_t)actor);
	}  
}
void AxisMap::DeleteViewRange(ViewRange* actor, b2AABB& aabb)
{
	float lowX = aabb.lowerBound.x;//����͵��X   
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
//׼��������,��ʼ����ǰ��������Ϣ
void AxisMap::ReadyInitActorData()
{
	int index = 0;
	size_t size = m_XAxisActors.size();//��ȡ����ǰ�Ĵ�С
	size_t nowSize = m_RecordArray.size();//��ȡ����ǰ�Ĵ�С
	if (size != nowSize) { 
		if ((size - nowSize) > 0) {//��Ҫ��ӵ����
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
	InPosActors** actorData = m_RecordArray.data();//������������Ĵ�С
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
// ��ȡ����Χ�ڵ����к��ʵĽ�ɫ
int AxisMap::GetRangeActors(std::vector<ViewRange*>& outData, const b2AABB& range)
{ 
	std::vector<ViewRange*> lowRangeArr;//�͵�ƥ��ĵ� 
	outData.resize(0); 
	InPosActors posActor; 
	posActor.m_PositionX = range.upperBound.x;
	int index = 0;
	auto lowPointItorSize = std::lower_bound(m_RecordArray.begin(), m_RecordArray.end(), &posActor, InPosActors()) - m_RecordArray.begin(); //���ж���X,���ж�һ��Y
	InPosActors** recordData = m_RecordArray.data();
	for (int i = 0; i < lowPointItorSize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList;
		lowRangeArr.resize(skipList->count);
		GetJumpListValue(skipList, lowRangeArr);//��ȡ�����з��������ĵ�  
		ViewRange** viewRangeArr = lowRangeArr.data();
		ViewRange* viewRangeTemp = NULL;
		for (int i = 0; i < lowRangeArr.size(); i++) {
			viewRangeTemp = viewRangeArr[i];
			b2AABB& AABB = viewRangeTemp->GetBodyAABB();
			//�ж�y��
			if (AABB.lowerBound.x > range.upperBound.x || AABB.lowerBound.y > range.upperBound.y || range.lowerBound.x > AABB.upperBound.x || range.lowerBound.y > AABB.upperBound.y)
				continue;
			outData.push_back(viewRangeTemp);
		}
	}
	//auto t1 = std::chrono::high_resolution_clock::now(); 
	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto sort1Cost = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << "     ��һ�������ʱ:" << sort1Cost << std::endl; 
	sort(outData.begin(), outData.end(), ViewRangeIDCompare); //����֮��,�������
	return outData.size();
} 

void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//�����ƶ����еĽ�ɫ
	bool isDelete = false;//��ǰ�Ƿ��������  
	auto iterEnd = m_DelayCalcMoveList.end();//��ֹ�ظ���ȡ��end
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { //ѭ�����������е��ƶ��Ľ�ɫ
		ActorID actorID = item->first; //��ȡ����ɫID
		actorView = item->second;//��ȡ����ɫ����ͼ����
		if(actorView->IsRefreshActorView() == 0)//�ƶ������ǳ
			continue;
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ��   
		m_AxisBodyMap.DeleteViewRange(actorView, actorView->GetBodyAABB());
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		m_AxisBodyMap.AddtionViewRange(actorView, actorView->GetBodyAABB());
		m_CalcMoveList.push_back(item->second);
	}  
	m_DelayCalcMoveList.clear(); 
} 
 
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{ 
	//׼����������
	m_AxisBodyMap.ReadyInitActorData(); 
	//���¼����Լ���������
	ViewRange* actoViewange = NULL;
	for (auto item = m_CalcMoveList.begin(); item != m_CalcMoveList.end(); item++) { 
		actoViewange = *item;
		m_AxisBodyMap.GetRangeActors(actoViewange->GetObserverArray(), actoViewange->GetViewAABB());
	}
	m_CalcMoveList.clear();   
} 