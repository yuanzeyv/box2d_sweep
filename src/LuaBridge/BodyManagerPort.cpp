#include "LuaBridge/BodyManagerPort.h" 
#include "Manager/BodyManager.h" 
void BodyManagerPort::InitBodyManager(sol::table solTable)
{
	auto BodyManagerTable = solTable.new_usertype<BodyManager>("BodyMan");
	BodyManagerTable["Instance"] = &BodyManager::Instance;
	BodyManagerTable["CreateBody"] = &BodyManager::CreateBody;
	BodyManagerTable["GetBody"] = &BodyManager::GetBody;
	BodyManagerTable["GetBodyList"] = &BodyManager::GetBodyList;
	BodyManagerTable["RegisterBody"] = &BodyManager::RegisterBody;
	BodyManagerTable["DeleteBody"] = sol::overload([](BodyManager* self, ActorID ID) {
		self->DeleteBody(ID);
		},
		[](BodyManager* self, b2Body* body) {
			self->DeleteBody(body);
		}); 
}
void BodyManagerPort::InitTable(sol::table table) 
{
	InitBodyManager(table);
} 