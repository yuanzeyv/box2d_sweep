#pragma
#include "box2d/box2d.h"
#include <unordered_map> 
#include "Library/sol/sol.hpp" 
using namespace std;
enum ColliderType {
	BeginContact = 0,//开始碰撞
	EndContact = 1,//结束碰撞
	PreSolve = 2,//碰撞产生 时
	PostSolve = 3,//碰撞处理时
};
class ColliderListenerManager :public b2ContactListener
{
public:
	unordered_map<ColliderType, sol::function> ContactMap;
public:
	virtual void inline BeginContact(b2Contact* contact);
	virtual void inline EndContact(b2Contact* contact);
	virtual void inline PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void inline PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	inline static ColliderListenerManager& Instance();
	inline static void RegisterColliderFunction(ColliderType type, sol::function func);
	inline static void UnregisterColliderFunction(ColliderType type);
};

inline void ColliderListenerManager::BeginContact(b2Contact* contact)
{
	if (ContactMap.count(ColliderType::BeginContact) == 0)
	{
		return;
	}
	ContactMap[ColliderType::BeginContact](contact);
}
inline void ColliderListenerManager::EndContact(b2Contact* contact)
{
	if (ContactMap.count(ColliderType::EndContact) == 0)
	{
		return;
	}
	ContactMap[ColliderType::EndContact](contact); 
}
inline void ColliderListenerManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	if (ContactMap.count(ColliderType::PreSolve) == 0)
	{
		return;
	}
	ContactMap[ColliderType::PreSolve](contact); 
}
inline void ColliderListenerManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	if (ContactMap.count(ColliderType::PostSolve) > 0)
	{
		ContactMap[ColliderType::PostSolve](contact);
	}
}
inline ColliderListenerManager& ColliderListenerManager::Instance()
{
	static ColliderListenerManager manager;
	return manager;
}
inline void ColliderListenerManager::RegisterColliderFunction(ColliderType type, sol::function func) {
	ColliderListenerManager::Instance().ContactMap[type] = func;
}

inline void ColliderListenerManager::UnregisterColliderFunction(ColliderType type) {
	if (ColliderListenerManager::Instance().ContactMap.count(type) == 0)
	{
		return ;
	}
	ColliderListenerManager::Instance().ContactMap.erase(type);
}
