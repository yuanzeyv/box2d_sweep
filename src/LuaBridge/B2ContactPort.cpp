#include "LuaBridge/B2ContactPort.h"  
#include "Box2d/box2d.h"

void B2ContactPort::InitB2ContactEdgeTable(sol::table solTable)
{
	auto b2ContactEdgeTable = solTable.new_usertype<b2ContactEdge>("b2ContactEdge");
	b2ContactEdgeTable["other"] = &b2ContactEdge::other;
	b2ContactEdgeTable["contact"] = &b2ContactEdge::contact;
	b2ContactEdgeTable["prev"] = &b2ContactEdge::prev;
	b2ContactEdgeTable["next"] = &b2ContactEdge::next;
}
void B2ContactPort::InitB2ContactTable(sol::table solTable)
{
	auto b2ContactTable = solTable.new_usertype<b2Contact>("b2Contact");
	b2ContactTable["GetWorldManifold"] = &b2Contact::GetWorldManifold;
	b2ContactTable["IsTouching"] = &b2Contact::IsTouching;
	b2ContactTable["SetEnabled"] = &b2Contact::SetEnabled;
	b2ContactTable["IsEnabled"] = &b2Contact::IsEnabled;
	b2ContactTable["GetChildIndexA"] = &b2Contact::GetChildIndexA;
	b2ContactTable["GetChildIndexB"] = &b2Contact::GetChildIndexB;
	b2ContactTable["SetFriction"] = &b2Contact::SetFriction;
	b2ContactTable["GetFriction"] = &b2Contact::GetFriction;
	b2ContactTable["ResetFriction"] = &b2Contact::ResetFriction;
	b2ContactTable["SetRestitution"] = &b2Contact::SetRestitution;
	b2ContactTable["GetRestitution"] = &b2Contact::GetRestitution;
	b2ContactTable["ResetRestitution"] = &b2Contact::ResetRestitution;
	b2ContactTable["SetRestitutionThreshold"] = &b2Contact::SetRestitutionThreshold;
	b2ContactTable["ResetRestitutionThreshold"] = &b2Contact::ResetRestitutionThreshold;
	b2ContactTable["SetTangentSpeed"] = &b2Contact::SetTangentSpeed;
	b2ContactTable["GetTangentSpeed"] = &b2Contact::GetTangentSpeed;
	b2ContactTable["Evaluate"] = &b2Contact::Evaluate;
	b2ContactTable["GetManifold"] = [](b2Contact* self)->b2Manifold* {return self->GetManifold();};
	b2ContactTable["GetNext"] = [](b2Contact* self)->b2Contact* {return self->GetNext();};
	b2ContactTable["GetFixtureA"] = [](b2Contact* self)->b2Fixture* {return self->GetFixtureA();};
	b2ContactTable["GetFixtureB"] = [](b2Contact* self)->b2Fixture* {return self->GetFixtureB();};
	b2ContactTable["GetBodyA"] = [](b2Contact* self)->b2Body* {return self->GetFixtureA()->GetBody();};
	b2ContactTable["GetBodyB"] = [](b2Contact* self)->b2Body* {return self->GetFixtureB()->GetBody();};
}
 
void B2ContactPort::InitTable(sol::table solTable)
{ 
	InitB2ContactEdgeTable(solTable);
	InitB2ContactTable(solTable); 
}