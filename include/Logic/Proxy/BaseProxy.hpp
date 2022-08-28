#pragma once
#include "PureMVC/PureMVC.hpp"   
struct BaseProxy : public PureMVC::Patterns::Proxy
{
    char const* NAME;
    static char const* const ON_REGISTER_CALLED;
    static char const* const ON_REMOVE_CALLED;

    BaseProxy(const char* name):NAME(name), PureMVC::Patterns::Proxy(NAME, "")//当前的代理名称
    { }

    virtual void onRegister(void)
    {
        setData(ON_REGISTER_CALLED);
    }

    virtual void onRemove(void)
    {
        setData(ON_REMOVE_CALLED);
    }
};  
char const* const BaseProxy::ON_REGISTER_CALLED = "onRegister Called";
char const* const BaseProxy::ON_REMOVE_CALLED = "onRemove Called";