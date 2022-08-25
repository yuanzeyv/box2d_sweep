#include "LuaBridge/B2FixturePort.h"
#include "Box2d/box2d.h"   
void B2FixturePort::InitB2Filter(sol::table solTable)
{
	auto b2FilterTable = solTable.new_usertype<b2Filter>("b2Filter");
	b2FilterTable["categoryBits"] = &b2Filter::categoryBits;
	b2FilterTable["maskBits"] = &b2Filter::maskBits;
	b2FilterTable["groupIndex"] = &b2Filter::groupIndex;
}
void B2FixturePort::InitB2FixtureDef(sol::table solTable)
{
	auto b2FixtureDefTable = solTable.new_usertype<b2FixtureDef>("b2FixtureDef");
	b2FixtureDefTable["shape"] = &b2FixtureDef::shape;
	b2FixtureDefTable["userData"] = &b2FixtureDef::userData;
	b2FixtureDefTable["friction"] = &b2FixtureDef::friction;
	b2FixtureDefTable["restitution"] = &b2FixtureDef::restitution;
	b2FixtureDefTable["restitutionThreshold"] = &b2FixtureDef::restitutionThreshold;
	b2FixtureDefTable["density"] = &b2FixtureDef::density;
	b2FixtureDefTable["isSensor"] = &b2FixtureDef::isSensor;
	b2FixtureDefTable["filter"] = &b2FixtureDef::filter;
}

void B2FixturePort::InitB2FixtureProxy(sol::table solTable)
{
	auto b2FixtureProxyTable = solTable.new_usertype<b2FixtureProxy>("b2FixtureProxy");
	b2FixtureProxyTable["categoryBits"] = &b2FixtureProxy::aabb;
	b2FixtureProxyTable["maskBits"] = &b2FixtureProxy::fixture;
	b2FixtureProxyTable["groupIndex"] = &b2FixtureProxy::childIndex;
	b2FixtureProxyTable["proxyId"] = &b2FixtureProxy::proxyId;
}
void B2FixturePort::InitB2Fixture(sol::table solTable)
{
	auto b2FixtureTable = solTable.new_usertype<b2Fixture>("b2Fixture");
	b2FixtureTable["GetType"] = &b2Fixture::GetType;
	b2FixtureTable["GetShape"] = [](b2Fixture* self)->b2Shape* {return self->GetShape();};
	b2FixtureTable["SetSensor"] = &b2Fixture::SetSensor;
	b2FixtureTable["IsSensor"] = &b2Fixture::IsSensor;
	b2FixtureTable["SetFilterData"] = &b2Fixture::SetFilterData;
	b2FixtureTable["GetFilterData"] = &b2Fixture::GetFilterData;
	b2FixtureTable["Refilter"] = &b2Fixture::Refilter;
	b2FixtureTable["GetBody"] = [](b2Fixture* self)->b2Body* {return self->GetBody();};
	b2FixtureTable["GetNext"] = [](b2Fixture* self)->b2Fixture* {return self->GetNext();};
	b2FixtureTable["GetUserData"] = [](b2Fixture* self)->b2FixtureUserData& {return self->GetUserData();};
	b2FixtureTable["TestPoint"] = &b2Fixture::TestPoint;
	b2FixtureTable["RayCast"] = &b2Fixture::RayCast;
	b2FixtureTable["GetMassData"] = &b2Fixture::GetMassData;
	b2FixtureTable["SetDensity"] = &b2Fixture::SetDensity;
	b2FixtureTable["GetFriction"] = &b2Fixture::GetFriction;
	b2FixtureTable["SetFriction"] = &b2Fixture::SetFriction;
	b2FixtureTable["GetRestitution"] = &b2Fixture::GetRestitution;
	b2FixtureTable["SetRestitution"] = &b2Fixture::SetRestitution;
	b2FixtureTable["GetRestitutionThreshold"] = &b2Fixture::GetRestitutionThreshold;
	b2FixtureTable["SetRestitutionThreshold"] = &b2Fixture::SetRestitutionThreshold;
	b2FixtureTable["GetAABB"] = &b2Fixture::GetAABB;
	b2FixtureTable["GetChildCount"] = &b2Fixture::GetChildCount;
	b2FixtureTable["Dump"] = &b2Fixture::Dump;
}

void B2FixturePort::InitB2FixtureUserData(sol::table solTable)
{
	auto b2FixtureUserDataTable = solTable.new_usertype<b2FixtureUserData>("b2FixtureUserData");
	b2FixtureUserDataTable["pointer"] = &b2FixtureUserData::pointer;
}
void B2FixturePort::InitTable(sol::table table)
{
	InitB2Filter(table);
	InitB2FixtureDef(table);
	InitB2FixtureProxy(table);
	InitB2Fixture(table);
	InitB2FixtureUserData(table);
}
