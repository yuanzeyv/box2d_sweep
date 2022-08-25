#include "LuaBridge/PhysicsParsePort.h"  
#include "PySourceParse/CircleShapeBase.h"
#include "PySourceParse/FixtureBase.h"
#include "PySourceParse/PhysicsParse.h"
#include "PySourceParse/PolygonShapeBase.h"
#include "PySourceParse/ShapeBase.h"
void PhysicsParsePort::InitPhysicsParse(sol::table solTable)
{
	auto PhysicsParseTable = solTable.new_usertype<PhysicsParse>("PhysicsParse");
	PhysicsParseTable["Instance"] = &PhysicsParse::Instance;
	PhysicsParseTable["FindFixtureVector"] = &PhysicsParse::FindFixtureVector;
	PhysicsParseTable["InsertFixtureBase"] = &PhysicsParse::InsertFixtureBase;
	PhysicsParseTable["ParsePhysicsParse"] = &PhysicsParse::ParsePhysicsParse;
	PhysicsParseTable["CreateBody"] = sol::overload([](PhysicsParse* self, b2Body* body, string name)->b2Body* { return self->CreateBody(body, name);},
		[](PhysicsParse* self, b2World* world, string name, b2Vec2 pos = b2Vec2(0, 0), b2BodyType type = b2BodyType::b2_dynamicBody)->b2Body* { return self->CreateBody(world, name, pos, type);});
}
void PhysicsParsePort::InitFixtureBase(sol::table solTable)
{
	auto FixtureBaseTable = solTable.new_usertype<FixtureBase>("FixtureBase");
	FixtureBaseTable["Name"] = &FixtureBase::Name;
	FixtureBaseTable["IsSensor"] = &FixtureBase::IsSensor;//是否为传感器
	FixtureBaseTable["Density"] = &FixtureBase::Density;//夹具质量
	FixtureBaseTable["Friction"] = &FixtureBase::Friction;//夹具摩擦
	FixtureBaseTable["Restitution"] = &FixtureBase::Restitution;//夹具弹性
	FixtureBaseTable["CategoryBits"] = &FixtureBase::CategoryBits;//夹具屏蔽字
	FixtureBaseTable["GroupIndex"] = &FixtureBase::GroupIndex;
	FixtureBaseTable["MaskBits"] = &FixtureBase::MaskBits;
	FixtureBaseTable["Format"] = &FixtureBase::Format;//获取到格式
	FixtureBaseTable["Ratio"] = &FixtureBase::Ratio;//获取到身体缩放  
	FixtureBaseTable["ShapeVec"] = &FixtureBase::ShapeVec;//获取到身体缩放  
	FixtureBaseTable["InsertShape"] = &FixtureBase::InsertShape;//获取到身体缩放  
	FixtureBaseTable["GetFixtureDefines"] = &FixtureBase::GetFixtureDefines;//获取到身体缩放  
}
void PhysicsParsePort::InitShapeBaseMap(sol::table solTable)
{
	solTable.new_usertype<vector<ShapeBase*>>("ShapeBaseMap");
} 
void PhysicsParsePort::InitShapeBase(sol::table solTable)
{
	auto  ShapeBaseTable = solTable.new_usertype<ShapeBase>("ShapeBase");
	ShapeBaseTable["Shape"] = &ShapeBase::Shape;
	ShapeBaseTable["InitShape"] = &ShapeBase::InitShape;
}
void PhysicsParsePort::InitPolygonShapeBase(sol::table solTable)
{
	auto PolygonShapeBaseTable = solTable.new_usertype<PolygonShapeBase>("PolygonShapeBase");
	PolygonShapeBaseTable[sol::base_classes] = sol::bases<ShapeBase>();
	PolygonShapeBaseTable["PointVec"] = &PolygonShapeBase::PointVec;
}
void PhysicsParsePort::InitCircleShapeBase(sol::table solTable)
{
	auto CircleShapeBaseTable = solTable.new_usertype<CircleShapeBase>("CircleShapeBase");
	CircleShapeBaseTable[sol::base_classes] = sol::bases<ShapeBase>();
	CircleShapeBaseTable["Radius"] = &CircleShapeBase::Radius;
	CircleShapeBaseTable["X"] = &CircleShapeBase::X;
	CircleShapeBaseTable["Y"] = &CircleShapeBase::Y;
}

void PhysicsParsePort::InitTable(sol::table solTable)
{
	InitPhysicsParse(solTable);
	InitFixtureBase( solTable);
	InitShapeBaseMap(solTable);
	InitShapeBase(solTable);
	InitPolygonShapeBase(solTable);
	InitCircleShapeBase(solTable);
}
