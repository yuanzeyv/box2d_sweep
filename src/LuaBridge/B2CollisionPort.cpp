#include "LuaBridge/B2CollisionPort.h" 
#include "Box2d/box2d.h"
void B2CollisionPort::InitB2ContactFeature(sol::table solTable)
{
	auto b2ContactFeatureTable = solTable.new_usertype<b2ContactFeature>("b2ContactFeature");
	b2ContactFeatureTable["indexA"] = &b2ContactFeature::indexA;
	b2ContactFeatureTable["indexB"] = &b2ContactFeature::indexB;
	b2ContactFeatureTable["typeA"] = &b2ContactFeature::typeA;
	b2ContactFeatureTable["typeB"] = &b2ContactFeature::typeB;
}
void B2CollisionPort::InitB2ManifoldPoint(sol::table solTable)
{
	auto b2ManifoldPointTable = solTable.new_usertype<b2ManifoldPoint>("b2ManifoldPoint");
	b2ManifoldPointTable["localPoint"] = &b2ManifoldPoint::localPoint;
	b2ManifoldPointTable["normalImpulse"] = &b2ManifoldPoint::normalImpulse;
	b2ManifoldPointTable["tangentImpulse"] = &b2ManifoldPoint::tangentImpulse;
	b2ManifoldPointTable["id"] = &b2ManifoldPoint::id;
} 
void B2CollisionPort::InitB2Manifold(sol::table solTable)
{
	auto b2ManifoldTable = solTable.new_usertype<b2Manifold>("b2Manifold");
	b2ManifoldTable["points"] = &b2Manifold::points;
	b2ManifoldTable["localNormal"] = &b2Manifold::localNormal;
	b2ManifoldTable["localPoint"] = &b2Manifold::localPoint;
	b2ManifoldTable["type"] = &b2Manifold::type;
	b2ManifoldTable["pointCount"] = &b2Manifold::pointCount;
}

void B2CollisionPort::InitB2WorldManifold(sol::table solTable)
{
	auto b2WorldManifoldTable = solTable.new_usertype<b2WorldManifold>("b2WorldManifold");
	b2WorldManifoldTable["Initialize"] = &b2WorldManifold::Initialize;
	b2WorldManifoldTable["normal"] = &b2WorldManifold::normal;
	b2WorldManifoldTable["points"] = &b2WorldManifold::points;
	b2WorldManifoldTable["separations"] = &b2WorldManifold::separations;
}
void B2CollisionPort::InitB2ClipVertex(sol::table solTable)
{
	auto b2ClipVertexTable = solTable.new_usertype<b2ClipVertex>("b2ClipVertex");
	b2ClipVertexTable["v"] = &b2ClipVertex::v;
	b2ClipVertexTable["id"] = &b2ClipVertex::id;
}
void B2CollisionPort::InitB2RayCastInput(sol::table solTable)
{
	auto b2RayCastInputTable = solTable.new_usertype<b2RayCastInput>("b2ClipVertex");
	b2RayCastInputTable["p1"] = &b2RayCastInput::p1;
	b2RayCastInputTable["p2"] = &b2RayCastInput::p2;
	b2RayCastInputTable["maxFraction"] = &b2RayCastInput::maxFraction;
}
void B2CollisionPort::InitB2RayCastOutput(sol::table solTable)
{
	auto b2RayCastOutputTable = solTable.new_usertype<b2RayCastOutput>("b2RayCastOutput");
	b2RayCastOutputTable["normal"] = &b2RayCastOutput::normal;
	b2RayCastOutputTable["fraction"] = &b2RayCastOutput::fraction;
} 
void B2CollisionPort::InitB2AABB(sol::table solTable)
{
	sol::usertype<b2AABB> userType = solTable.new_usertype<b2AABB>("b2AABB", sol::constructors<b2AABB()>());
	userType["IsValid"] = &b2AABB::IsValid;
	userType["Contains"] = &b2AABB::Contains;
	userType["GetPerimeter"] = &b2AABB::GetPerimeter;
	userType["GetCenter"] = &b2AABB::GetCenter;
	userType["GetExtents"] = &b2AABB::GetExtents;
	userType["lowerBound"] = &b2AABB::lowerBound;
	userType["upperBound"] = &b2AABB::upperBound;
	userType["RayCast"] = &b2AABB::RayCast;
	userType["CombineOne"] = sol::overload(
		[](b2AABB& self, b2AABB& aabb)->void {
			self.Combine(aabb);
		}, [](b2AABB& self, b2AABB& aabb1, b2AABB& aabb2)->void {
			self.Combine(aabb1, aabb2);
		}); 
}
 
void B2CollisionPort::InitTable(sol::table solTable)
{
	InitB2ContactFeature(solTable);
	InitB2ManifoldPoint(solTable);
	InitB2Manifold(solTable);
	InitB2WorldManifold(solTable);
	InitB2ClipVertex(solTable);
	InitB2RayCastInput(solTable);
	InitB2AABB(solTable);
} 