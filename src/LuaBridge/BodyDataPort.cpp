#include "LuaBridge/BodyDataPort.h" 
#include "Manager/Base/BodyData.h" 
void BodyDataPort::InitBodyData(sol::table solTable)
{
	auto  BodyDataTable = solTable.new_usertype<BodyData>("BodyData", sol::constructors<BodyData(BodyType, b2Body*)>());
	BodyDataTable["CalcBodyAABB"] = &BodyData::CalcBodyAABB;
	BodyDataTable["CalcViewBody"] = &BodyData::CalcViewBody;
	BodyDataTable["GetAABB"] = &BodyData::GetAABB;
	BodyDataTable["GetBody"] = &BodyData::GetBody;
	BodyDataTable["GetViewAABB"] = &BodyData::GetViewAABB;
	BodyDataTable["ID"] = &BodyData::ID;
	BodyDataTable["Type"] = &BodyData::Type;
}
void BodyDataPort::InitBodyMap(sol::table solTable)
{
	auto mapTable = solTable.new_usertype<std::unordered_map<ActorID, BodyData*>>("BodyMap");
	mapTable["at"] = [](std::unordered_map<ActorID, BodyData*>* self, ActorID val)->BodyData* {
		if (self->find(val) == self->end())
			return NULL;
		return self->at(val);
	}; 
}
void BodyDataPort::InitTable(sol::table table)
{
	InitBodyData(table);
	InitBodyMap(table);
} 