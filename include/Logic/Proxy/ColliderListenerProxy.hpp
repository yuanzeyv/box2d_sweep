#pragma once 
#include "Logic/Proxy/BaseProxy.hpp"
#include "Library/TimeWheel/timer-wheel.h" 
#include "Library/sol/sol.hpp" 
#include "box2d/box2d.h"
#include <unordered_map> 
#include "Library/sol/sol.hpp" 
using namespace std;

class ColliderListenerProxy :public BaseProxy
{
public:
	unordered_map<ColliderType, sol::function> m_ContactMap;
public:
	virtual void inline BeginContact(b2Contact* contact);
	virtual void inline EndContact(b2Contact* contact);
	virtual void inline PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void inline PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	inline void RegisterColliderFunction(ColliderType type, sol::function func);
	inline void UnregisterColliderFunction(ColliderType type);
public:

}; 

inline void ColliderListenerProxy::BeginContact(b2Contact* contact)
{
	if (m_ContactMap.count(ColliderType::BEGIN_CONTACT) == 0)
	{
		return;
	}
	m_ContactMap[ColliderType::BEGIN_CONTACT](contact);
}
inline void ColliderListenerProxy::EndContact(b2Contact* contact)
{
	if (m_ContactMap.count(ColliderType::END_CONTACT) == 0)
	{
		return;
	}
	m_ContactMap[ColliderType::END_CONTACT](contact);
}
inline void ColliderListenerProxy::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	if (m_ContactMap.count(ColliderType::PRE_SOLVE) == 0)
	{
		return;
	}
	m_ContactMap[ColliderType::PRE_SOLVE](contact);
}
inline void ColliderListenerProxy::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	if (m_ContactMap.count(ColliderType::POST_SOLVE) > 0)
	{
		m_ContactMap[ColliderType::POST_SOLVE](contact);
	}
}  
inline void ColliderListenerProxy::RegisterColliderFunction(ColliderType type, sol::function func) {
	m_ContactMap[type] = func;
}

inline void ColliderListenerProxy::UnregisterColliderFunction(ColliderType type) {
	if (m_ContactMap.count(type) == 0)
	{
		return;
	}
	m_ContactMap.erase(type);
}
