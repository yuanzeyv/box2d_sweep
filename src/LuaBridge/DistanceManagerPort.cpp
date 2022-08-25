#include "LuaBridge/DistanceManagerPort.h"  
#include "Manager/DistanceManager.h"
void DistanceManagerPort::InitDistanceManager(sol::table solTable)
{
	auto DistanceManagerTable = solTable.new_usertype<DistanceManager>("DistanceMan");
	DistanceManagerTable["Instance"] = &DistanceManager::Instance;
	DistanceManagerTable["GetViewBody"] = [](DistanceManager* self, ActorID id, ViewType type, ViewStatus view)->std::unordered_map<ActorID, BodyData*>*{
		auto& mapTable = self->GetViewBody(id, type, view);
		if (mapTable.size() == 0)
			return NULL;
		return &mapTable;
	};
	DistanceManagerTable["DistanceDump"] = &DistanceManager::DistanceDump;

}
void DistanceManagerPort::InitVisibleMap(sol::table solTable)
{
	solTable.new_usertype<std::unordered_map<ActorID, BodyData*>>("VisibleMap");
}
 
void DistanceManagerPort::InitTable(sol::table table)
{
	InitDistanceManager(table);
	InitVisibleMap(table);
}
