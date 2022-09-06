#include "test.h"
#include <stdio.h>
#include <map>
#include <string>
#include <unordered_map>
#include <chrono>
#include <iostream> 
#include "Manager/TemplateBoard.h"
#include "Manager/BodyManager.h"   
#include "Manager/AxisDistanceManager.h"  
#include "LuaBridge/LuaBridge.h"
#include "Navmesh/Navmesh.h"
#include "Manager/NavmeshManager.h"
#include "Manager/TimeWheelManager.h"
#include "Manager/PlayerManager.h"
using namespace std;  
class Crash : public Test
{
public:
	b2Body* Player;
	LuaBridge Lua; 
	void InitLua()
	{
		Lua.LuaState.open_libraries(sol::lib::package, sol::lib::base, sol::lib::debug, sol::lib::math, sol::lib::string, sol::lib::os, sol::lib::io, sol::lib::table, sol::lib::coroutine);
		Lua.LuaState["package"]["path"] = "./tests/Scripts/?.lua";
		auto result = Lua.LuaState.do_file("./tests/Scripts/main.lua", sol::load_mode::any);
		if (!result.valid())
		{
			printf("加载初始化文件错误\n\r");
			exit(0);
		}
		//NavmeshManager& data = NavmeshManager::Instance();
		//NavmeshID id = data.GenerateNavmesh("a");
		//for (int i = 0; i < 200;i++)
		//{
		//	data.RegisterActor(id, i);
		//} 
		//vector<PosV3> startRand;
		//vector<PosV3> endRand;
		//for (int i = 0; i < 200;i++)
		//{
		//	startRand.push_back(PosV3());
		//	endRand.push_back(PosV3()); 
		//	data.FindRandPoint(id, startRand[i]);
		//	data.FindRandPoint(id, endRand[i]);
		//} 
		//auto t1 = chrono::high_resolution_clock::now();
		//for (int i = 0; i < 200;i++)
		//{
		//	//data.Recast(i, b2Vec2(startRand[i].X, startRand[i].Z), b2Vec2(endRand[i].X, endRand[i].Z));
		// 	//data.FindPath(i, b2Vec2(startRand[i].X, startRand[i].Z), b2Vec2(endRand[i].X, endRand[i].Z)); 
		//	data.SlicedFindPath(i, b2Vec2(startRand[i % 3 ].X, startRand[i %3].Z), b2Vec2(endRand[i%3 ].X, endRand[i%3 ].Z));
		//}
		//auto t2 = chrono::high_resolution_clock::now();
		//long long int  a = chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count(); 
		//printf("执行5000次射线的时间为:%lld\n\r", a);
	}
	Crash::~Crash()
	{
		Lua.LuaState["Exit"]();
		Test::~Test(); 
	}
	Crash():m_TimeWheelManager(TimeWheelManager::Instance()), Player(NULL)
	{
		BodyManager::Instance().SetWorld(m_world);
		InitLua(); 
	}  
	static Test* Create()
	{
		return new Crash;
	}
	
	void DrawShape(b2Body* body)
	{
		ViewRange* viewRange = AxisDistanceManager::Instance().GetViewRange(body->ID()); 
		auto visibleMap = viewRange->GetVisibleMap();
		 
		__DrawShape(visibleMap);
	}
	void __DrawShape(unordered_map<int64_t, ViewRange*>& bodyMap)
	{   
		for (auto item = bodyMap.begin(); item != bodyMap.end(); item++)
		{
			const BodyData* bodyBase = item->second->GetActor();
			const b2Body* body = bodyBase->GetBody();
			const b2Transform& xf = body->GetTransform();
			for (const b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
			{
				if (body->GetType() == b2_dynamicBody && body->GetMass() == 0.0f)
				{
					m_world->DrawShape(f, xf, b2Color(1.0f, 0.0f, 0.0f));
				}
				else if (body->IsEnabled() == false)
				{
					m_world->DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.3f));
				}
				else if (body->GetType() == b2_staticBody)
				{
					m_world->DrawShape(f, xf, b2Color(0.5f, 0.9f, 0.5f));
				}
				else if (body->GetType() == b2_kinematicBody)
				{
					m_world->DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.9f));
				}
				else if (body->IsAwake() == false)
				{
					m_world->DrawShape(f, xf, b2Color(0.6f, 0.6f, 0.6f));
				}
				else
				{
					m_world->DrawShape(f, xf, b2Color(0.9f, 0.7f, 0.7f));
				}
			}
			auto& bodyAABB = item->second->GetBodyAABB(PointType::BODY_TYPE);
			b2Vec2 vs[4];
			vs[0].Set(bodyAABB.lowerBound.x, bodyAABB.lowerBound.y);
			vs[1].Set(bodyAABB.upperBound.x, bodyAABB.lowerBound.y);
			vs[2].Set(bodyAABB.upperBound.x, bodyAABB.upperBound.y);
			vs[3].Set(bodyAABB.lowerBound.x, bodyAABB.upperBound.y);
			m_world->m_debugDraw->DrawPolygon(vs, 4, b2Color(0.9f, 0.1f, 0.1f)); 
		} 
	}
	TimeWheelManager& m_TimeWheelManager; 
	virtual void DebugDraw(b2World* world,float step)
	{ 
		Lua.LuaState["Tick"](step);//tick完成之后，进行距离计算
		NavmeshManager::Instance().Update(step); 
		m_TimeWheelManager.Update(step);  
		AxisDistanceManager::Instance().RecalcActorDistance();
		if (Player) {
			DrawShape(Player); 

			b2Vec2 pos = Player->GetPosition();//当前的点位
			pos.y = -pos.y;//找到正确的点位
			float distance = NavmeshManager::Instance().Recast(Player->ID(), pos, b2Vec2(30, -30));//从当前点到 终点的距离
			b2Vec2 endPoint = (b2Vec2(30, -30) - pos);
			endPoint *= distance;
			endPoint += pos;
			m_world->m_debugDraw->DrawSegment(Player->GetPosition(), b2Vec2(endPoint.x, -endPoint.y), b2Color(255, 20, 0));
		}
	}
	virtual void Keyboard(int key)
	{
		if (key == 263) {
			Player = PlayerManager::Instance().GetChooseBody();
		} 
		PlayerID id = 0;
		if (Player) {
			id = Player->ID();
		}
		Lua.LuaState["Input"]((char)key, id);//tick完成之后，进行距离计算 
	} 
};

static int testIndex = RegisterTest("Chapter", "crash", Crash::Create); 