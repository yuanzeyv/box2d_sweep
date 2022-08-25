#include "LuaBridge/B2BroadPhasePort.h"   
#include "Box2d/box2d.h"
void B2BroadPhasePort::InitB2Pair(sol::table solTable)
{
	auto b2PairTable = solTable.new_usertype<b2Pair>("b2Pair");
	b2PairTable["proxyIdA"] = &b2Pair::proxyIdA;
	b2PairTable["proxyIdB"] = &b2Pair::proxyIdB; 
}

void  B2BroadPhasePort::InitB2BroadPhase(sol::table solTable)
{
	auto b2BroadPhaseTable = solTable.new_usertype<b2BroadPhase>("b2BroadPhase");
	b2BroadPhaseTable["CreateProxy"] = &b2BroadPhase::CreateProxy;
	b2BroadPhaseTable["DestroyProxy"] = &b2BroadPhase::DestroyProxy;
	b2BroadPhaseTable["MoveProxy"] = &b2BroadPhase::MoveProxy;
	b2BroadPhaseTable["TouchProxy"] = &b2BroadPhase::TouchProxy;
	b2BroadPhaseTable["GetFatAABB"] = &b2BroadPhase::GetFatAABB;
	b2BroadPhaseTable["GetUserData"] = &b2BroadPhase::GetUserData;
	b2BroadPhaseTable["TestOverlap"] = &b2BroadPhase::TestOverlap;
	b2BroadPhaseTable["GetProxyCount"] = &b2BroadPhase::GetProxyCount;
	b2BroadPhaseTable["GetTreeHeight"] = &b2BroadPhase::GetTreeHeight;
	b2BroadPhaseTable["GetTreeBalance"] = &b2BroadPhase::GetTreeBalance;
	b2BroadPhaseTable["GetTreeQuality"] = &b2BroadPhase::GetTreeQuality;
	b2BroadPhaseTable["ShiftOrigin"] = &b2BroadPhase::ShiftOrigin; 
}
void B2BroadPhasePort::InitTable(sol::table table)
{ 
	InitB2Pair(table);
	InitB2BroadPhase(table);
}
