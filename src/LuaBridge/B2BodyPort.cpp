#include "LuaBridge/B2BodyPort.h"   
#include "Box2d/box2d.h"
void B2BodyPort::InitB2BodyDef(sol::table solTable)
{
	auto b2BodyDefTable = solTable.new_usertype<b2BodyDef>("b2BodyDef");
	b2BodyDefTable["type"] = &b2BodyDef::type;
	b2BodyDefTable["position"] = &b2BodyDef::position;
	b2BodyDefTable["angle"] = &b2BodyDef::angle;
	b2BodyDefTable["linearVelocity"] = &b2BodyDef::linearVelocity;
	b2BodyDefTable["angularVelocity"] = &b2BodyDef::angularVelocity;
	b2BodyDefTable["linearDamping"] = &b2BodyDef::linearDamping;
	b2BodyDefTable["angularDamping"] = &b2BodyDef::angularDamping;
	b2BodyDefTable["allowSleep"] = &b2BodyDef::allowSleep;
	b2BodyDefTable["awake"] = &b2BodyDef::awake;
	b2BodyDefTable["fixedRotation"] = &b2BodyDef::fixedRotation;
	b2BodyDefTable["bullet"] = &b2BodyDef::bullet;
	b2BodyDefTable["enabled"] = &b2BodyDef::enabled;
	b2BodyDefTable["userData"] = &b2BodyDef::userData;
	b2BodyDefTable["gravityScale"] = &b2BodyDef::gravityScale;
}

void B2BodyPort::InitB2Body(sol::table solTable)
{
	auto b2BodyTable = solTable.new_usertype<b2Body>("b2Body");
	b2BodyTable["CreateFixture"] = sol::overload([](b2Body* self, const b2FixtureDef* def)->b2Fixture* { return self->CreateFixture(def); },
		[](b2Body* self, const b2Shape* shape, float density)->b2Fixture* { return self->CreateFixture(shape, density); });
	b2BodyTable["DestroyFixture"] = &b2Body::DestroyFixture;
	b2BodyTable["SetTransform"] = &b2Body::SetTransform;
	b2BodyTable["GetTransform"] = &b2Body::GetTransform;
	b2BodyTable["GetPosition"] = &b2Body::GetPosition;
	b2BodyTable["GetAngle"] = &b2Body::GetAngle;
	b2BodyTable["GetWorldCenter"] = &b2Body::GetWorldCenter;
	b2BodyTable["GetLocalCenter"] = &b2Body::GetLocalCenter;
	b2BodyTable["SetLinearVelocity"] = &b2Body::SetLinearVelocity;
	b2BodyTable["GetLinearVelocity"] = &b2Body::GetLinearVelocity;
	b2BodyTable["SetAngularVelocity"] = &b2Body::SetAngularVelocity;
	b2BodyTable["GetAngularVelocity"] = &b2Body::GetAngularVelocity;
	b2BodyTable["ApplyForce"] = &b2Body::ApplyForce;
	b2BodyTable["ApplyForceToCenter"] = &b2Body::ApplyForceToCenter;
	b2BodyTable["ApplyTorque"] = &b2Body::ApplyTorque;;
	b2BodyTable["ApplyLinearImpulse"] = &b2Body::ApplyLinearImpulse;;
	b2BodyTable["ApplyLinearImpulseToCenter"] = &b2Body::ApplyLinearImpulseToCenter;
	b2BodyTable["ApplyAngularImpulse"] = &b2Body::ApplyAngularImpulse;
	b2BodyTable["GetMass"] = &b2Body::GetMass;
	b2BodyTable["GetInertia"] = &b2Body::GetInertia;
	b2BodyTable["GetMassData"] = &b2Body::GetMassData;
	b2BodyTable["SetMassData"] = &b2Body::SetMassData;
	b2BodyTable["ResetMassData"] = &b2Body::ResetMassData;
	b2BodyTable["GetWorldPoint"] = &b2Body::GetWorldPoint;
	b2BodyTable["GetWorldVector"] = &b2Body::GetWorldVector;
	b2BodyTable["GetLocalPoint"] = &b2Body::GetLocalPoint;
	b2BodyTable["GetLocalVector"] = &b2Body::GetLocalVector;
	b2BodyTable["GetLinearVelocityFromWorldPoint"] = &b2Body::GetLinearVelocityFromWorldPoint;
	b2BodyTable["GetLinearVelocityFromLocalPoint"] = &b2Body::GetLinearVelocityFromLocalPoint;
	b2BodyTable["GetLinearDamping"] = &b2Body::GetLinearDamping;
	b2BodyTable["SetLinearDamping"] = &b2Body::SetLinearDamping;
	b2BodyTable["GetAngularDamping"] = &b2Body::GetAngularDamping;
	b2BodyTable["SetAngularDamping"] = &b2Body::SetAngularDamping;
	b2BodyTable["GetGravityScale"] = &b2Body::GetGravityScale;
	b2BodyTable["SetGravityScale"] = &b2Body::SetGravityScale;
	b2BodyTable["SetType"] = &b2Body::SetType;
	b2BodyTable["GetType"] = &b2Body::GetType;
	b2BodyTable["SetBullet"] = &b2Body::SetBullet;
	b2BodyTable["IsBullet"] = &b2Body::IsBullet;
	b2BodyTable["SetSleepingAllowed"] = &b2Body::SetSleepingAllowed;
	b2BodyTable["IsSleepingAllowed"] = &b2Body::IsSleepingAllowed;
	b2BodyTable["SetAwake"] = &b2Body::SetAwake;
	b2BodyTable["IsAwake"] = &b2Body::IsAwake;
	b2BodyTable["SetEnabled"] = &b2Body::SetEnabled;
	b2BodyTable["IsEnabled"] = &b2Body::IsEnabled;
	b2BodyTable["SetFixedRotation"] = &b2Body::SetFixedRotation;

	b2BodyTable["GetFixtureList"] = [](b2Body* self)->b2Fixture* {
		return self->GetFixtureList();
	};
	//b2BodyTable["GetJointList"] = sol::overload(&b2Body::GetJointList);
	//b2BodyTable["GetContactList"] = sol::overload(&b2Body::GetContactList);
	//b2BodyTable["GetNext"] = sol::overload(&b2Body::GetNext);
	//b2BodyTable["GetUserData"] = sol::overload(&b2Body::GetUserData);
	//b2BodyTable["GetWorld"] = sol::overload(&b2Body::GetWorld);
	b2BodyTable["Dump"] = &b2Body::Dump;
	b2BodyTable["Splite"] = &b2Body::Splite;
	b2BodyTable["SetPosition"] = sol::overload([](b2Body* self, float x, float y) { return self->SetPosition(x, y); },
		[](b2Body* self, b2Vec2 pos) { return self->SetPosition(pos); });
	b2BodyTable["ID"] = &b2Body::ID;
	b2BodyTable["GetWorldGravity"] = [](b2Body* self)->b2Vec2 { return self->GetWorld()->GetGravity(); };

}
void B2BodyPort::InitTable(sol::table solTable)
{ 
	InitB2Body(solTable);
	InitB2BodyDef(solTable);
}
