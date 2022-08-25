#include "box2d/box2d.h"
#include "Library/xml/tinyxml2.h"
#include "PySourceParse/PhysicsParse.h" 
#include "PySourceParse/CircleShapeBase.h"
#include "PySourceParse/PolygonShapeBase.h"
#include <vector>
using namespace tinyxml2;
using namespace std;
PhysicsParse& PhysicsParse::Instance() {
	static PhysicsParse Instance;
	return Instance;
}
void PhysicsParse::InsertFixtureBase(FixtureBase& base) {
	BodyModelMap[base.Name].push_back(base);
}
vector<FixtureBase>* PhysicsParse::FindFixtureVector(string name)
{
	if (BodyModelMap.find(name) == BodyModelMap.end())
		return NULL;
	return &BodyModelMap[name];
} 
//私有构造器
PhysicsParse::PhysicsParse() {
}
void PhysicsParse::ParsePhysicsParse(string path)
{
	XMLDocument doc;
	doc.LoadFile(path.c_str());
	assert(doc.ErrorID() == XML_SUCCESS); //验证文件是否加载成功 0代表成功  
	XMLElement* rootNode = doc.RootElement();//获取到当前的根 
	string version = rootNode->FindAttribute("version")->Value();//获取到当前的版本信息
	XMLElement* metadata = rootNode->FirstChildElement("metadata");//获取到缩放比例
	float format = metadata->FirstChildElement("format")->FloatText();//获取到格式
	float ptm_ratio = metadata->FirstChildElement("ptm_ratio")->FloatText();//获取到身体缩放 

	XMLElement* bodies = rootNode->FirstChildElement("bodies");//获取到身体列表 
	for (XMLElement* bodyElement = bodies->FirstChildElement(); bodyElement; bodyElement = bodyElement->NextSiblingElement())
	{
		//创建一个FixtureBase
		string bodyName = bodyElement->Attribute("name")  ;
		for (XMLElement* element = bodyElement->FirstChildElement("fixtures")->FirstChildElement(); element; element = element->NextSiblingElement())
		{
			FixtureBase FixtureCell; 
			FixtureCell.Density = element->FirstChildElement("density")->FloatText();
			FixtureCell.Friction = element->FirstChildElement("friction")->FloatText();
			FixtureCell.Restitution = element->FirstChildElement("restitution")->FloatText();
			FixtureCell.CategoryBits = element->FirstChildElement("filter_categoryBits")->IntText();
			FixtureCell.GroupIndex = element->FirstChildElement("filter_groupIndex")->IntText();
			FixtureCell.MaskBits = element->FirstChildElement("filter_maskBits")->IntText();
			FixtureCell.IsSensor = element->FirstChildElement("isSensor") != NULL;
			FixtureCell.Name = bodyName;
			FixtureCell.Ratio = ptm_ratio;
			FixtureCell.Format = format;
						 
			ShapeBase* ShapeCell = NULL;
			string type = element->FirstChildElement("fixture_type")->GetText();//获取到当前的形状
			if (type == "CIRCLE")
			{
				XMLElement* CircleElement = element->FirstChildElement("circle");
				CircleShapeBase* CircleBodyCell = new CircleShapeBase();
				CircleBodyCell->Radius = CircleElement->FloatAttribute("r") / ptm_ratio;
				CircleBodyCell->X = CircleElement->FloatAttribute("x") / ptm_ratio;
				CircleBodyCell->Y = CircleElement->FloatAttribute("y") / ptm_ratio; 
				FixtureCell.InsertShape(CircleBodyCell);
			}
			else if (type == "POLYGON") {
				XMLElement* shapeFirst = element->FirstChildElement("polygons")->FirstChildElement();//首先获取到形状总表
				for (XMLElement* shapeElement = shapeFirst; shapeElement; shapeElement = shapeElement->NextSiblingElement())
				{
					PolygonShapeBase* PolygonBodyCell = new PolygonShapeBase();
					const char* posStr = shapeElement->GetText();
					char* posStrTemp = new char[strlen(posStr) + 1];
					strcpy(posStrTemp, posStr);
					char* ptr = NULL;
					char* token = NULL;
					float xPos, yPos;
					token = strtok_s(posStrTemp, ",", &ptr);
					do {
						xPos = atof(token) / ptm_ratio;
						token = strtok_s(NULL, ",", &ptr);
						yPos = atof(token) / ptm_ratio;
						token = strtok_s(NULL, ",", &ptr);
						PolygonBodyCell->PointVec.push_back(b2Vec2(xPos, yPos));
					} while (token); 

					FixtureCell.InsertShape(PolygonBodyCell);
				}
			}
			PhysicsParse::Instance().InsertFixtureBase(FixtureCell);
		}
	}
}

b2Body* PhysicsParse::CreateBody(b2Body* body, string name)
{ 
	vector<FixtureBase>* Data = FindFixtureVector(name);
	if (Data == NULL)//寻找当前这个对象
		return NULL;
	//调用创建fixture的方法，将所有夹具创建完成
	for(auto var = Data->begin(); var != Data->end();var ++ ){
		vector<b2FixtureDef> FixtureVec = var->GetFixtureDefines();
		for (int i = 0; i < FixtureVec.size(); i++)
			body->CreateFixture(&FixtureVec[i]);
	} 
	return body;
}
b2Body* PhysicsParse::CreateBody(b2World* world, string name, b2Vec2 pos,b2BodyType type)
{ 
	if (!FindFixtureVector(name))//寻找当前是否拥有这个 对象
		return NULL;
	b2BodyDef BodyDef;
	BodyDef.position = pos;
	b2Body* Body = world->CreateBody(&BodyDef);//创建一个Body 
	Body->SetType(type);
	return CreateBody(Body, name); 
}