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
	m_mouseJoint = NULL; //���ؽ�
	m_bomb = NULL;
	m_textLine = 30;
	m_textIncrement = 13;
	m_pointCount = 0;//��ײ����Ϣ

	m_destructionListener.test = this;//��ǰ�ļ�������
	m_world->SetDestructionListener(&m_destructionListener);//���õ�ǰ�ļ�������
	m_world->SetContactListener(&ColliderListenerManager::Instance());//������ײ����
	m_world->SetDebugDraw(&g_debugDraw);//���õ��Ի��Ƶ�Ԫ

	m_bombSpawning = false;

	m_stepCount = 0;//���е�֡��
	b2BodyDef nullDef = b2BodyDef();
	m_groundBody = m_world->CreateBody(&nullDef);//����һ��ȫ�ֿն���

	memset(&m_maxProfile, 0, sizeof(b2Profile));//��ǰ��������Ϣ
	memset(&m_totalProfile, 0, sizeof(b2Profile));

}

Test::~Test()
{ 
	delete m_world;//ɾ������
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
		b2Body* body = fixture->GetBody();//��ȡ����ǰ��
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
	m_world->QueryAABB(&callback, aabb);//��ѯ���������ļҾ� 
	if (callback.body)
	{
		float frequencyHz = 5.0f;
		float dampingRatio = 0.7f;  
		b2MouseJointDef jd;//����һ�����ؽ�
		jd.bodyA = m_groundBody;//�Ե�
		jd.bodyB = callback.body;//�Թؽ�
		jd.target = p;//λ��
		jd.maxForce = 1000.0f * callback.body->GetMass();//�϶�����
		b2LinearStiffness(jd.stiffness, jd.damping, frequencyHz, dampingRatio, jd.bodyA, jd.bodyB);//�������Ըն� 
		m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&jd);//��������ؽ�
		callback.body->SetAwake(true);//���Ѹ���
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
	m_world->SetContinuousPhysics(settings.m_enableContinuous);//��������������
	m_world->SetSubStepping(settings.m_enableSubStepping);
	double start, end, cost; 
	start = clock();
	m_pointCount = 0;//��ײ��  
	m_world->Step(timeStep, settings.m_velocityIterations, settings.m_positionIterations); //��������Ļ��� 
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
		int32 contactCount = m_world->GetContactCount();//��ײ��
		int32 jointCount = m_world->GetJointCount();//�ؽ���
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
