#include "Manager/AxisDistanceManager.h" 

#include<queue>
using namespace std;    
ViewRange::ViewRange(BodyData* bodyData, b2Vec2 range) :
	m_ObserverRange(range),
	m_BodyPos(MAX_DISTANCE, MAX_DISTANCE),
	m_Actor(bodyData), 
	m_OffsetCondtion(-9999,-9999),
	m_ViewObserverStateGenerate(20)
{
	m_FrontObserverPoint = &m_ObserverArr[0];//��һ��ԭʼ������һ֡���Կ�����
	m_ObserverPoint = &m_ObserverArr[1];//��һ��ԭʼ������һ֡���Կ����� 
	m_FrontBeObserverPoint = &m_BeObserverArr[0];//��һ��ԭʼ������һ֡���Կ�����
	m_BeObserverPoint = &m_BeObserverArr[1];//��һ��ԭʼ������һ֡���Կ����� 
}
void ViewRange::ChangeObserverStatus(ViewRange* ObserverActor, bool status)
{ 
	ActorID actorID = ObserverActor->GetActor()->ID();
	auto observerDataPoint = m_ObserverPoint->data();
	auto findData = lower_bound(observerDataPoint, observerDataPoint + m_ObserverPoint->size(), actorID, [](ViewObserverState* actor, ActorID id)->bool {return actor->m_ViewRange->GetActor()->ID() < id; });
	ViewObserverState* statePoint = NULL;
	if ((findData - observerDataPoint) != m_ObserverPoint->size() && (*findData)->m_ViewRange->GetActor()->ID() == actorID) {
		statePoint = *findData;
	}
	else if (m_ExtraObserverArr.count(actorID)) {
		statePoint = m_ExtraObserverArr[actorID];
	}
	if (statePoint == NULL && status == 1)
		return;
	if (statePoint == NULL) {
		statePoint = m_ViewObserverStateGenerate.RequireObj();
		statePoint->m_IsVisible = 0;
		statePoint->m_ViewRange = ObserverActor;
		m_ExtraObserverArr[actorID] = statePoint;
		return;
	}
	statePoint->m_IsVisible = status;
}

void ViewRange::ChangeBeObserverStatus(ViewRange* beObserverActor, bool status)
{
	ActorID actorID = beObserverActor->GetActor()->ID();
	auto observerDataPoint = m_BeObserverPoint->data();
	auto findData = lower_bound(observerDataPoint, observerDataPoint + m_BeObserverPoint->size(), actorID, [](ViewObserverState* actor, ActorID id)->bool {return actor->m_ViewRange->GetActor()->ID() < id; });
	ViewObserverState * statePoint = NULL;
	if ((findData - observerDataPoint) != m_BeObserverPoint->size() && (*findData)->m_ViewRange->GetActor()->ID() == actorID) {
		statePoint = *findData;
	}else if (m_ExtraBeObserverArr.count(actorID)) {
		statePoint = m_ExtraBeObserverArr[actorID];
	}
	if (statePoint == NULL && status == 1)
		return;
	if (statePoint == NULL) {
		statePoint = m_ViewObserverStateGenerate.RequireObj();
		statePoint->m_IsVisible = 0;
		statePoint->m_ViewRange = beObserverActor;
		m_ExtraBeObserverArr[actorID] = statePoint;
		return;
	}
	statePoint->m_IsVisible = status;
}
void ViewRange::ClearExtraObserverArr()//ɾ�����ж���Ĺ۲����
{
	for (auto item = m_ExtraObserverArr.begin();item != m_ExtraObserverArr.end();item++) {
		m_ViewObserverStateGenerate.BackObj(item->second);
	}
	m_ExtraObserverArr.clear();
}

void ViewRange::ClearExtraBeObserverArr()//ɾ�����ж���Ĺ۲����
{
	for (auto item = m_ExtraBeObserverArr.begin();item != m_ExtraBeObserverArr.end();item++) {
		m_ViewObserverStateGenerate.BackObj(item->second);
	}
	m_ExtraBeObserverArr.clear();
}

AxisMap::~AxisMap() {
	for (int i = 0; i < PointType::MAX_POINT_TYPE;i++) {
		for (int j = 0;j < m_RecordArray[i].size();j++) {
			m_InPosActorsGenerate.BackObj(m_RecordArray[i][j]);
		}
		m_XAxisActors[i].clear();
		m_RecordArray[i].resize(0);
	}
}

void AxisMap::AddtionViewRange(PointType type, ViewRange* actor, b2AABB& aabb)
{
	float upX = aabb.upperBound.x;//����͵��X 
	ActorID actorId = actor->GetActor()->ID();
	if (m_XAxisActors[type].count(upX) == 0)
		m_XAxisActors[type][upX] = skiplist_new();
	auto jumpList = m_XAxisActors[type][upX];
	if (!skiplist_search(jumpList, actorId)){//����
		skiplist_insert(jumpList, actorId,(int64_t)actor);
	}  
}
 
void AxisMap::DeleteViewRange(PointType type,ViewRange* actor, b2AABB& aabb)
{
	float upX = aabb.upperBound.x;//����͵��X   
	ActorID actorId = actor->GetActor()->ID(); 
	if (m_XAxisActors[type].count(upX) == 0)
		return;
	auto jumpList = m_XAxisActors[type][upX];
	skiplist_remove(jumpList, actorId);
	if (jumpList->count == 0) {
		skiplist_delete(jumpList);
		m_XAxisActors[type].erase(upX);
	}  
}
//׼��������,��ʼ����ǰ��������Ϣ
void AxisMap::ReadyInitActorData()
{ 
	for (int i = 0; i < PointType::MAX_POINT_TYPE;i++) {
		int index = 0;
		size_t size = m_XAxisActors[i].size();//��ȡ����ǰ�Ĵ�С
		size_t nowSize = m_RecordArray[i].size();//��ȡ����ǰ�Ĵ�С
		if (size != nowSize) {
			if ((size - nowSize) > 0) {//��Ҫ��ӵ����
				m_RecordArray[i].resize(size);
				InPosActors** actorData = m_RecordArray[i].data();
				for (int index = nowSize;index < size;index++)
					actorData[index] = m_InPosActorsGenerate.RequireObj();
			}
			else {
				InPosActors** actorData = m_RecordArray[i].data();
				for (int index = nowSize;index < size;index++)
					m_InPosActorsGenerate.BackObj(actorData[index]);
				m_RecordArray[i].resize(size);
			}
		}
		InPosActors** actorData = m_RecordArray[i].data();//������������Ĵ�С
		auto itorEnd = m_XAxisActors[i].end();
		for (auto actorSet = m_XAxisActors[i].begin(); actorSet != itorEnd; actorSet++, index++) {
			actorData[index]->m_PositionX = actorSet->first;
			actorData[index]->m_SkipList = actorSet->second;
		}
	}
} 
static bool ViewRangeIDCompare(ViewRange* node1, ViewRange* node2)
{ 
	return node1->GetActor()->ID() < node2->GetActor()->ID();
} 
// ��ȡ����Χ�ڵ����к��ʵĽ�ɫ
int AxisMap::SetRangeActors(PointType type, PointType compareType, ViewRange* actor,std::vector<ViewObserverState*>& outData, const b2AABB& range)
{ 
	InPosActors posActor; 
	posActor.m_PositionX = range.lowerBound.x; 
	auto lowPointItorSize = std::lower_bound(m_RecordArray[type].begin(), m_RecordArray[type].end(), &posActor, InPosActors()) - m_RecordArray[type].begin(); //���ж���X,���ж�һ��Y
	InPosActors** recordData = m_RecordArray[type].data();
	size_t RecordArraySize =  m_RecordArray[type].size();
	size_t realSize = 0; 
	for (int i = lowPointItorSize; i < RecordArraySize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList;
		realSize += skipList->count;
	}
	std::vector<ViewRange*> tempData(realSize);
	ViewRange** tempDataPoint = tempData.data();
	ViewRange* viewRangeTemp = NULL;
	realSize = 0;
	for (int i = lowPointItorSize; i < RecordArraySize; i++) {
		skiplist* skipList = recordData[i]->m_SkipList; 
		struct sk_link* pos = &skipList->head[0];
		struct sk_link* end = &skipList->head[0];
		pos = pos->next;
		for (; pos != end; pos = pos->next) { 
			viewRangeTemp = (ViewRange*)list_entry(pos, struct skipnode, link[0])->value; 
			b2AABB& AABB = PointType::BODY_TYPE == compareType ? viewRangeTemp->GetBodyAABB ():viewRangeTemp->GetViewAABB();
			if (AABB.lowerBound.y > range.upperBound.y || AABB.lowerBound.x > range.upperBound.x || range.lowerBound.y > AABB.upperBound.y)
				continue;
			tempDataPoint[realSize++] = viewRangeTemp;
		} 
	} 
	 
	if(realSize != tempData.size())
		tempData.resize(realSize); 
	sort(tempDataPoint, tempDataPoint + realSize, ViewRangeIDCompare);//��ͼ�ڹ۲�Ķ���Ҳ������̫��
	size_t nowSize = outData.size();//��ȡ����ǰ�Ĵ�С
	if (nowSize < realSize)
		outData.resize(realSize);
	ViewObserverState** outDataArr = outData.data();
	size_t loopCount = max(realSize, nowSize);
	for (int i = 0; i < loopCount;i++) {
		if (i < realSize) {//���õĻ�
			if (!outDataArr[i]) outDataArr[i] = actor->m_ViewObserverStateGenerate.RequireObj();
			outDataArr[i]->m_IsVisible = 0;//�ɼ�
			outDataArr[i]->m_ViewRange = tempDataPoint[i];
		} else if(outDataArr[i])
			actor->m_ViewObserverStateGenerate.BackObj(outDataArr[i]);
	}
	if (nowSize > realSize)
		outData.resize(realSize);//�������� 
	return outData.size();
} 

void AxisDistanceManager::ActorsMove()
{
	ViewRange* actorView = NULL;//�����ƶ����еĽ�ɫ
	bool isDelete = false;//��ǰ�Ƿ��������  
	auto iterEnd = m_DelayCalcMoveList.end();//��ֹ�ظ���ȡ��end
	size_t moveCount = 0;
	size_t delayCount = m_DelayCalcMoveList.size();
	m_CalcMoveList.resize(delayCount);
	ViewRange** moveListData = m_CalcMoveList.data();
	for (auto item = m_DelayCalcMoveList.begin(); item != iterEnd;item++) { //ѭ�����������е��ƶ��Ľ�ɫ
		ActorID actorID = item->first; //��ȡ����ɫID
		actorView = item->second;//��ȡ����ɫ����ͼ����
		if(actorView->IsRefreshActorView() == 0)//�ƶ������ǳ
			continue;
		actorView->RecalcBodyPosition();//�ؼ����ɫ��λ��   
		m_AxisBodyMap->DeleteViewRange(PointType::BODY_TYPE, actorView, actorView->GetBodyAABB());
		m_AxisBodyMap->DeleteViewRange(PointType::VIEW_TYPE, actorView, actorView->GetViewAABB());
		actorView->RecalcBodyAABB();
		actorView->RecalcViewAABB();
		m_AxisBodyMap->AddtionViewRange(PointType::BODY_TYPE, actorView, actorView->GetBodyAABB());
		m_AxisBodyMap->AddtionViewRange(PointType::VIEW_TYPE, actorView, actorView->GetViewAABB());
		moveListData[moveCount++] = item->second;
	}  
	if(delayCount != moveCount)
		m_CalcMoveList.resize(moveCount);//�������ô�С
	m_DelayCalcMoveList.clear(); 
} 
inline static bool ViewRangePointComapre(ViewRange* v1, ViewRange* v2)
{ 
	return v1->GetActor()->ID() < v2->GetActor()->ID();
} 

inline static void DifferenceVector(vector<vector<ViewRange*>*> differnceList, vector<ViewRange*>& outData) {
	//��һ���� �ڶ��� ������ ������
	vector<ViewRange*>** typeArr = differnceList.data();
	size_t size = differnceList.size();
	vector<ViewRange*>* orign = typeArr[0];
	if (orign->size() == 0) {
		outData.resize(0);
		return;
	}
	vector<ViewRange*> tempArr[2];
	int pointArr[2] = { 0,1 };//0�ǵ�ǰ  1��ǰһ��
	for (int i = 1;i < size;i++) {
		int nowPoint = pointArr[0];
		tempArr[nowPoint].resize(max(orign->size(), differnceList[i]->size()));
		tempArr[nowPoint].resize(set_difference(orign->data(), orign->data() + orign->size(),
			differnceList[i]->data(), differnceList[i]->data() + differnceList[i]->size(), tempArr[pointArr[0]].data(), ViewRangePointComapre) - tempArr[nowPoint].data());
		orign = &tempArr[nowPoint]; 
		pointArr[0] = pointArr[1];
		pointArr[1] = nowPoint;
	}
	outData.resize(orign->size());
	if(outData.size() != 0)
		copy(orign->data(), orign->data() + orign->size(), outData.data());
}
void AxisDistanceManager::CalcObserver(vector<ViewRange*>& moveRangeArr)//�ؼ��� 
{
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> frontObserverCalcArr;
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> observerCalcArr;  
	int moveSize = moveRangeArr.size(); 
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	vector<ViewRange*> finalDifferenceArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ 
	for (int i = 0;i < moveSize;i++) { 
		actoViewange = moveArrData[i];
		size_t frontSize = actoViewange->m_FrontObserverPoint->size();
		frontObserverCalcArr.resize(frontSize + actoViewange->m_ExtraObserverArr.size());
		ViewRange** tempFrontObserverArr = frontObserverCalcArr.data(); 
		ViewObserverState** frontObserverArr = actoViewange->m_FrontObserverPoint->data();
		size_t observerSize = 0;
		for (int pos = 0;pos < frontSize;pos++) {
			if (frontObserverArr[pos]->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[observerSize++] = frontObserverArr[pos]->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		size_t midIndex = observerSize ;
		for (auto item = actoViewange->m_ExtraObserverArr.begin();item != actoViewange->m_ExtraObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[observerSize++] = item->second->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		frontObserverCalcArr.resize(observerSize);
		tempFrontObserverArr = frontObserverCalcArr.data();
		inplace_merge(tempFrontObserverArr, tempFrontObserverArr + midIndex, tempFrontObserverArr + observerSize, ViewRangePointComapre);
		//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ 
		observerSize = actoViewange->m_ObserverPoint->size();
		observerCalcArr.resize(observerSize);
		ViewObserverState** actorObserverArr = actoViewange->m_ObserverPoint->data();
		ViewRange**  nowObserverArr = observerCalcArr.data();
		for (int pos = 0;pos < observerSize;pos++) {
			nowObserverArr[pos] = actorObserverArr[pos]->m_ViewRange;
		}
		//ǰһ֡ �� ��֡ȡ�. �õ�Ӧ�ñ�����Ϊ���صĵ�Ԫ 
		DifferenceVector({ &frontObserverCalcArr ,&observerCalcArr ,&moveRangeArr }, finalDifferenceArr); 
		auto finalDiffernceArrData = finalDifferenceArr.data();
		auto finalSize = finalDifferenceArr.size();
		for(int i = 0;i < finalSize;i++) 
			finalDiffernceArrData[i]->ChangeBeObserverStatus(actoViewange, 1); //������󽫲��ڱ� acto���۲�

		DifferenceVector({ &observerCalcArr ,&frontObserverCalcArr ,&moveRangeArr }, finalDifferenceArr); 
		finalDiffernceArrData = finalDifferenceArr.data();
		finalSize = finalDifferenceArr.size();
		for (int i = 0;i < finalSize;i++)
			finalDiffernceArrData[i]->ChangeBeObserverStatus(actoViewange,0); //������󽫲��ڱ� acto���۲�
		actoViewange->ClearExtraBeObserverArr();
	}
}

void AxisDistanceManager::CalcBeObserver(vector<ViewRange*>& moveRangeArr)//�ؼ��� 
{
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> frontBeObserverCalcArr;
	//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ
	vector<ViewRange*> beObserverCalcArr; 
	int moveSize = moveRangeArr.size();
	ViewRange** moveArrData = moveRangeArr.data();
	ViewRange* actoViewange = NULL;
	vector<ViewRange*> finalDifferenceArr;//ͨ����ɼ��б�ȡ�,�޳��ޱ仯����Ա��Ϣ 
	for (int i = 0;i < moveSize;i++) {
		actoViewange = moveArrData[i];
		size_t frontSize = actoViewange->m_FrontBeObserverPoint->size();
		frontBeObserverCalcArr.resize(actoViewange->m_FrontBeObserverPoint->size() + actoViewange->m_ExtraBeObserverArr.size());
		ViewRange** tempFrontObserverArr = frontBeObserverCalcArr.data();
		ViewObserverState** frontObserverArr = actoViewange->m_FrontBeObserverPoint->data();
		size_t beObserverSize = 0;

		for (int pos = 0;pos < frontSize;pos++) {
			if (frontObserverArr[pos]->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[beObserverSize++] = frontObserverArr[pos]->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		size_t midIndex = beObserverSize;
		for (auto item = actoViewange->m_ExtraBeObserverArr.begin();item != actoViewange->m_ExtraBeObserverArr.end();item++) {
			if (item->second->m_IsVisible == 1)
				continue;
			tempFrontObserverArr[beObserverSize++] = item->second->m_ViewRange;//�ɼ��ż���,������Ϊ��ɾ����
		}
		frontBeObserverCalcArr.resize(beObserverSize);
		tempFrontObserverArr = frontBeObserverCalcArr.data();
		inplace_merge(tempFrontObserverArr, tempFrontObserverArr + midIndex, tempFrontObserverArr + beObserverSize, ViewRangePointComapre);
		//ȡ�ö�����һ֡���۲�ĵ������е���Ϣ 
		beObserverSize = actoViewange->m_BeObserverPoint->size();
		beObserverCalcArr.resize(beObserverSize);
		ViewObserverState** actorBeObserverArr = actoViewange->m_BeObserverPoint->data();
		ViewRange** nowBeObserverArr = beObserverCalcArr.data();
		for (int pos = 0;pos < beObserverSize;pos++) {
			nowBeObserverArr[pos] = actorBeObserverArr[pos]->m_ViewRange;
		}
		//ǰһ֡ �� ��֡ȡ�. �õ�Ӧ�ñ�����Ϊ���صĵ�Ԫ
		DifferenceVector({ &frontBeObserverCalcArr ,&beObserverCalcArr ,&moveRangeArr }, finalDifferenceArr);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++) 
			(*item)->ChangeObserverStatus(actoViewange, 1); //���õ�ǰ����δ����״̬ 

		DifferenceVector({ &beObserverCalcArr ,&frontBeObserverCalcArr ,&moveRangeArr }, finalDifferenceArr);
		for (auto item = finalDifferenceArr.begin();item != finalDifferenceArr.end();item++)
			(*item)->ChangeObserverStatus(actoViewange, 0); //���õ�ǰ����δ����״̬ 
		actoViewange->ClearExtraObserverArr();
	}
}
//�ؼ���
void AxisDistanceManager::CalcViewObj()
{
	if (m_CalcMoveList.size() <= 0) return; 
	m_AxisBodyMap->ReadyInitActorData();  
	int moveSize = m_CalcMoveList.size(); 
	ViewRange** moveArrData = m_CalcMoveList.data();  
	ViewRange* actoViewange = NULL;
	for (int i = 0;i < moveSize;i++) { //�ؼ�����ҵ���Ϣ
		actoViewange = moveArrData[i];
		actoViewange->ShiftObserverPoint();//������һ֡����
		m_AxisBodyMap->SetRangeActors(PointType::BODY_TYPE, PointType::BODY_TYPE, actoViewange, *actoViewange->m_ObserverPoint, actoViewange->GetViewAABB());//���¼��㱾֡�۲�
		actoViewange->ShiftBeObserverPoint();//������һ֡����
		m_AxisBodyMap->SetRangeActors(PointType::VIEW_TYPE, PointType::VIEW_TYPE, actoViewange, *actoViewange->m_BeObserverPoint, actoViewange->GetBodyAABB());//ȡ�õ�ǰ��˭�۲�����
	}
	//����������Ĳ����,�ƶ��Ķ���,�����¼����˹۲��뱻�۲��������Ϣ.�����������ݲ��������
	CalcBeObserver(m_CalcMoveList);
	CalcObserver(m_CalcMoveList);
	//m_CalcMoveList.clear();//�����,��Լ����
}