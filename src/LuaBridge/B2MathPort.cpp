#include "LuaBridge/B2MathPort.h"  
#include "Box2d/box2d.h"
void B2MathPort::Initb2Vec2(sol::table solTable)
{
	solTable.new_usertype<b2Vec2>("b2Vec2", sol::constructors<b2Vec2(), b2Vec2(float, float)>(),
		"SetZero", &b2Vec2::SetZero,
		//"Set", sol::overload(&b2Vec2::Set),
		"Length", &b2Vec2::Length,
		"LengthSquared", &b2Vec2::LengthSquared,
		"Normalize", &b2Vec2::Normalize,
		"IsValid", &b2Vec2::IsValid,
		"Skew", &b2Vec2::Skew,
		"x", &b2Vec2::x,
		"y", &b2Vec2::y
		);
}
void B2MathPort::InitTable(sol::table solTable)
{ 
	Initb2Vec2(solTable);
}