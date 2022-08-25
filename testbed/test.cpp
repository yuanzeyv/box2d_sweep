#include "test.h"
#include "settings.h"
#include <stdio.h>
#include <time.h>   
#include <cstdint>
#include "Manager/Base/BodyData.h"
#include "Manager/DistanceManager.h"
using namespace std; 
void DestructionListener::SayGoodbye(b2Joint* joint)
{ 
	test->m_mouseJoint = NULL; 
}
Test::Test()
{ 
	m_world = new b2World(b2Vec2(0.0f, 0.0f));
	m_mouseJoint = NULL; //鼠标关节
	m_bomb = NULL;
	m_textLine = 30;
	m_textIncrement = 13;
	m_pointCount = 0;//碰撞点信息

	m_destructionListener.test = this;//当前的监听对象
	m_world->SetDestructionListener(&m_destructionListener);//设置当前的监听对象
	m_world->SetContactListener(&ColliderListenerManager::Instance());//设置碰撞对向
	m_world->SetDebugDraw(&g_debugDraw);//设置调试绘制单元

	m_bombSpawning = false;

	m_stepCount = 0;//运行到帧数
	b2BodyDef nullDef = b2BodyDef();
	m_groundBody = m_world->CreateBody(&nullDef);//创建一个全局空对象

	memset(&m_maxProfile, 0, sizeof(b2Profile));//当前的数据信息
	memset(&m_totalProfile, 0, sizeof(b2Profile));

}

Test::~Test()
{ 
	delete m_world;//删除世界
	m_world = NULL;
} 
 
class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		body = NULL;
	} 
	bool ReportFixture(b2Fixture* fixture) override
	{
		b2Body* body = fixture->GetBody();//获取到当前的
		if (body->GetType() == b2_dynamicBody){
			bool inside = fixture->TestPoint(m_point);
			if (inside){
				body = fixture->GetBody();
				return false;// We are done, terminate the query.
			}
		}  
		return true;// Continue the query.
	} 
	b2Vec2 m_point;
	b2Body* body;
};

void Test::MouseDown(const b2Vec2& p)
{
	m_mouseWorld = p; 
	if (m_mouseJoint != NULL) 
		return; 
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d(0.001f, 0.001f); 
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d; 
	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	m_world->QueryAABB(&callback, aabb);//查询符合条件的家具 
	if (callback.body)
	{
		float frequencyHz = 5.0f;
		float dampingRatio = 0.7f;  
		b2MouseJointDef jd;//创建一个鼠标关节
		jd.bodyA = m_groundBody;//对地
		jd.bodyB = callback.body;//对关节
		jd.target = p;//位置
		jd.maxForce = 1000.0f * callback.body->GetMass();//拖动力度
		b2LinearStiffness(jd.stiffness, jd.damping, frequencyHz, dampingRatio, jd.bodyA, jd.bodyB);//计算线性刚度 
		m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&jd);//创建这个关节
		callback.body->SetAwake(true);//唤醒刚体
	}
}
  
void Test::MouseUp(const b2Vec2& p)
{
	if (m_mouseJoint)
	{
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	} 
}

void Test::MouseMove(const b2Vec2& p)
{
	m_mouseWorld = p;
	
	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	}
}
void Test::DebugDraw(b2World* world,float step)
{

} 
void Test::Step(Settings& settings)
{ 
	float timeStep = settings.m_hertz > 0.0f ? 1.0f / settings.m_hertz : float(0.0f);

	if (settings.m_pause)
	{
		if (settings.m_singleStep) 
			settings.m_singleStep = 0; 
		else 
			timeStep = 0.0f; 

		g_debugDraw.DrawString(5, m_textLine, "****PAUSED****");
		m_textLine += m_textIncrement;
	}

	uint32 flags = 0;
	flags += settings.m_drawShapes * b2Draw::e_shapeBit;
	flags += settings.m_drawJoints * b2Draw::e_jointBit;
	flags += settings.m_drawAABBs * b2Draw::e_aabbBit;
	flags += settings.m_drawCOMs * b2Draw::e_centerOfMassBit;
	g_debugDraw.SetFlags(flags);

	m_world->SetAllowSleeping(settings.m_enableSleep);
	m_world->SetWarmStarting(settings.m_enableWarmStarting);
	m_world->SetContinuousPhysics(settings.m_enableContinuous);//开启连续物理检测
	m_world->SetSubStepping(settings.m_enableSubStepping);
	double start, end, cost; 
	start = clock();
	m_pointCount = 0;//碰撞点  
	m_world->Step(timeStep, settings.m_velocityIterations, settings.m_positionIterations); //运行世界的绘制 
	end = clock();
	cost = end - start;
	DebugDraw(m_world, timeStep);
	//printf("%f/n", cost);
    g_debugDraw.Flush();

	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}

	if (settings.m_drawStats)
	{
		int32 bodyCount = m_world->GetBodyCount();
		int32 contactCount = m_world->GetContactCount();//碰撞点
		int32 jointCount = m_world->GetJointCount();//关节舒
		g_debugDraw.DrawString(5, m_textLine, "bodies/contacts/joints = %d/%d/%d", bodyCount, contactCount, jointCount);
		m_textLine += m_textIncrement;

		int32 proxyCount = m_world->GetProxyCount();
		int32 height = m_world->GetTreeHeight();
		int32 balance = m_world->GetTreeBalance();
		float quality = m_world->GetTreeQuality();
		g_debugDraw.DrawString(5, m_textLine, "proxies/height/balance/quality = %d/%d/%d/%g", proxyCount, height, balance, quality);
		m_textLine += m_textIncrement;
	} 
}

void Test::ShiftOrigin(const b2Vec2& newOrigin)
{
	m_world->ShiftOrigin(newOrigin);
}

TestEntry g_testEntries[MAX_TESTS] = { {nullptr} };
int g_testCount = 0;

int RegisterTest(const char* category, const char* name, TestCreateFcn* fcn)
{ 
	if (g_testCount < MAX_TESTS)
	{
		g_testEntries[g_testCount] = { category, name, fcn }; 
		return g_testCount++;
	} 
	return -1;
}
