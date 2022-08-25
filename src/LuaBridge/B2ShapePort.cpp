#include "LuaBridge/B2ShapePort.h" 
#include "Box2d/box2d.h" 
void B2ShapePort::InitB2MassData(sol::table solTable)
{
	auto b2MassDataTable = solTable.new_usertype<b2MassData>("b2MassData");
	b2MassDataTable["mass"] = &b2MassData::mass;
	b2MassDataTable["center"] = &b2MassData::center;
	b2MassDataTable["I"] = &b2MassData::I;
}

void B2ShapePort::InitB2Shape(sol::table solTable)
{
	auto b2ShapeTable = solTable.new_usertype<b2Shape>("b2Shape");
	b2ShapeTable["Clone"] = &b2Shape::Clone;
	b2ShapeTable["GetType"] = &b2Shape::GetType;
	b2ShapeTable["TestPoint"] = &b2Shape::TestPoint;
	b2ShapeTable["RayCast"] = &b2Shape::RayCast;
	b2ShapeTable["ComputeAABB"] = &b2Shape::ComputeAABB;
	b2ShapeTable["ComputeMass"] = &b2Shape::ComputeMass;
	b2ShapeTable["GetRealCircle"] = [](b2Shape* self)->b2CircleShape* {
		return (b2CircleShape*)self;
	};
	b2ShapeTable["GetRealPolygon"] = [](b2Shape* self)->b2PolygonShape* {
		return (b2PolygonShape*)self;
	};
	b2ShapeTable["ComputeMass"] = &b2Shape::ComputeMass;
	b2ShapeTable["m_type"] = &b2Shape::m_type;
	b2ShapeTable["m_radius"] = &b2Shape::m_radius;
}
void B2ShapePort::InitB2ChainShape(sol::table solTable)
{
	auto b2ChainShapeTable = solTable.new_usertype<b2ChainShape>("b2ChainShape");
	b2ChainShapeTable[sol::base_classes] = sol::bases<b2Shape>();
	b2ChainShapeTable["Clear"] = &b2ChainShape::Clear;
	b2ChainShapeTable["CreateLoop"] = &b2ChainShape::CreateLoop;
	b2ChainShapeTable["CreateChain"] = &b2ChainShape::CreateChain;
	b2ChainShapeTable["GetChildEdge"] = &b2ChainShape::GetChildEdge;
	b2ChainShapeTable["m_prevVertex"] = &b2ChainShape::m_prevVertex;
	b2ChainShapeTable["m_nextVertex"] = &b2ChainShape::m_nextVertex;
	b2ChainShapeTable["m_vertices"] = &b2ChainShape::m_vertices;
	b2ChainShapeTable["m_count"] = &b2ChainShape::m_count;
}
void B2ShapePort::InitB2CircleShape(sol::table solTable)
{
	auto b2CircleShapeTable = solTable.new_usertype<b2CircleShape>("b2CircleShape");
	b2CircleShapeTable[sol::base_classes] = sol::bases<b2Shape>();
	b2CircleShapeTable["m_p"] = &b2CircleShape::m_p;
	b2CircleShapeTable["GetVectors"] = [](b2CircleShape* self, int count = 8)->std::vector<b2Vec2> {//获取到周边的点 个数为圆的周边点数
		double angle = 360 / count;
		std::vector<b2Vec2> points;
		for (int i = 0; i < count;i++) {
			float posX = self->m_radius * cos(b2_pi * 2 * i / count) + self->m_p.x;
			float posY = self->m_radius * sin(b2_pi * 2 * i / count) + self->m_p.y;
			points.push_back(b2Vec2(posX, posY));
		}
		return points;
	};
}
void B2ShapePort::InitB2EdgeShape(sol::table solTable)
{
	auto b2EdgeShapeTable = solTable.new_usertype<b2EdgeShape>("b2EdgeShape");
	b2EdgeShapeTable[sol::base_classes] = sol::bases<b2Shape>();
	b2EdgeShapeTable["SetOneSided"] = &b2EdgeShape::SetOneSided;
	b2EdgeShapeTable["SetTwoSided"] = &b2EdgeShape::SetTwoSided;
	b2EdgeShapeTable["m_vertex1"] = &b2EdgeShape::m_vertex1;
	b2EdgeShapeTable["m_vertex2"] = &b2EdgeShape::m_vertex2;
	b2EdgeShapeTable["m_vertex0"] = &b2EdgeShape::m_vertex0;
	b2EdgeShapeTable["m_vertex3"] = &b2EdgeShape::m_vertex3;
	b2EdgeShapeTable["m_oneSided"] = &b2EdgeShape::m_oneSided;
}
void B2ShapePort::InitB2PolygonShape(sol::table solTable)
{
	auto b2_polygon_shapeTable = solTable.new_usertype<b2PolygonShape>("b2PolygonShape");
	b2_polygon_shapeTable[sol::base_classes] = sol::bases<b2Shape>();
	b2_polygon_shapeTable["Set"] = &b2PolygonShape::Set;
	//b2_polygon_shapeTable["SetAsBox"] = sol::overload(&b2PolygonShape::SetAsBox);
	b2_polygon_shapeTable["Validate"] = &b2PolygonShape::Validate;
	b2_polygon_shapeTable["m_centroid"] = &b2PolygonShape::m_centroid;
	b2_polygon_shapeTable["m_vertices"] = &b2PolygonShape::m_vertices;
	b2_polygon_shapeTable["m_normals"] = &b2PolygonShape::m_normals;
	b2_polygon_shapeTable["m_count"] = &b2PolygonShape::m_count;
	b2_polygon_shapeTable["GetVectors"] = [](b2PolygonShape* self, int count = 0) -> std::vector<b2Vec2> {//获取到周边的点 个数为圆的周边点数 
		std::vector<b2Vec2> points;
		for (int i = 0; i < self->m_count; i++)
			points.push_back(self->m_vertices[i]);
		return points;
	};
}
void B2ShapePort::InitTable(sol::table solTable)  
{
	InitB2MassData(solTable);
	InitB2Shape(solTable);
	InitB2ChainShape(solTable);
	InitB2CircleShape(solTable);
	InitB2EdgeShape(solTable);
	InitB2PolygonShape(solTable);
} 