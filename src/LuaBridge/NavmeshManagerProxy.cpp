#include "LuaBridge/NavmeshManagerProxy.h"   
#include "Manager/NavmeshManager.h"
void NavmeshManagerProxy::InitNavmeshManager(sol::table solTable)
{
	auto NavmeshManagerTable = solTable.new_usertype<NavmeshManager>("NavmeshMan");
	NavmeshManagerTable["Instance"] = &NavmeshManager::Instance;
	NavmeshManagerTable["NavmeshMap"] = &NavmeshManager::NavmeshMap;
	NavmeshManagerTable["MapInner"] = &NavmeshManager::MapInner;
	NavmeshManagerTable["MapContain"] = &NavmeshManager::MapContain;
	NavmeshManagerTable["GenerateNavmesh"] = &NavmeshManager::GenerateNavmesh;
	NavmeshManagerTable["RemoveNavmesh"] = &NavmeshManager::RemoveNavmesh;
	NavmeshManagerTable["RegisterActor"] = &NavmeshManager::RegisterActor;
	NavmeshManagerTable["UnregisterActor"] = &NavmeshManager::UnregisterActor;
	NavmeshManagerTable["FindRandPoint"] = &NavmeshManager::FindRandPoint;
	NavmeshManagerTable["FindPath"] = &NavmeshManager::FindPath;
	NavmeshManagerTable["Recast"] = &NavmeshManager::Recast;
	NavmeshManagerTable["SlicedFindPath"] = &NavmeshManager::SlicedFindPath;

}  
void NavmeshManagerProxy::InitTable(sol::table solTable)
{
	InitNavmeshManager(solTable);
}
