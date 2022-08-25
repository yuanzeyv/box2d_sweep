#include "LuaBridge/ColliderListenerManagerPort.h" 
#include "Manager/ColliderListenerManager.h" 
void ColliderListenerManagerPort::InitColliderListenerManager(sol::table solTable)
{
	auto ColliderListenerManagerTable = solTable.new_usertype<ColliderListenerManager>("ColliderListenerManager");
	ColliderListenerManagerTable["RegisterColliderFunction"] = &ColliderListenerManager::RegisterColliderFunction;
	ColliderListenerManagerTable["UnregisterColliderFunction"] = &ColliderListenerManager::UnregisterColliderFunction;

}
  
void ColliderListenerManagerPort::InitTable(sol::table table) 
{
	InitColliderListenerManager(table);
} 