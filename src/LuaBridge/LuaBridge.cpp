#include "LuaBridge/B2BodyPort.h"
#include "LuaBridge/B2BroadPhasePort.h"
#include "LuaBridge/B2CollisionPort.h"
#include "LuaBridge/B2ContactPort.h"
#include "LuaBridge/B2FixturePort.h"
#include "LuaBridge/B2MathPort.h"
#include "LuaBridge/B2ShapePort.h"
#include "LuaBridge/BehaviorTreePort.h"
#include "LuaBridge/BodyDataPort.h"
#include "LuaBridge/BodyManagerPort.h"
#include "LuaBridge/ColliderListenerManagerPort.h"
#include "LuaBridge/CommonalityPort.h" 
#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/PhysicsParsePort.h"
#include "LuaBridge/DelabellaPort.h"
#include "LuaBridge/NavmeshManagerProxy.h"
#include "LuaBridge/TimeWheelPort.h"
#include "LuaBridge/PlayerManagerProxy.h"
using namespace  std;
LuaBridge::LuaBridge()
{
    this->RegisterSolPort(new B2BodyPort());
    this->RegisterSolPort(new B2BroadPhasePort());
    this->RegisterSolPort(new B2CollisionPort());   
    this->RegisterSolPort(new B2ContactPort());  
    this->RegisterSolPort(new B2FixturePort());  
    this->RegisterSolPort(new B2MathPort());
    this->RegisterSolPort(new B2ShapePort());
    this->RegisterSolPort(new BehaviorTreePort());
    this->RegisterSolPort(new BodyDataPort());
    this->RegisterSolPort(new BodyManagerPort());
    this->RegisterSolPort(new ColliderListenerManagerPort());
    this->RegisterSolPort(new CommonalityPort()); 
    this->RegisterSolPort(new PhysicsParsePort());
    this->RegisterSolPort(new DelabellaPort());
    this->RegisterSolPort(new NavmeshManagerProxy());
    this->RegisterSolPort(new TimeWheelPort());
    this->RegisterSolPort(new PlayerManagerProxy());
    
    sol::state_view lua(LuaState);
    sol::table table = lua.create_table("sol");
    for (auto item = SOLPortList.begin();item != SOLPortList.end();item++)
    {
        (*item)->InitTable(table);
    }
}   