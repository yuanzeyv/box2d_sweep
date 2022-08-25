local BaseProxy = require("Logic.Proxy.BaseProxy")
local ColliderProxy = class("ColliderProxy",BaseProxy)   
function ColliderProxy:InitData() 
end 
function ColliderProxy:LoadFinish()
    sol.ColliderListenerManager.RegisterColliderFunction(EnumDefine.ColliderType.BeginContact,function(contact)  
        xpcall(self.BeginContactHandle,__TRACKBACK__,self,contact)
    end)
    sol.ColliderListenerManager.RegisterColliderFunction(EnumDefine.ColliderType.EndContact,function(contact) 
        xpcall(self.EndContactHandle,__TRACKBACK__,self,contact)
    end)
    sol.ColliderListenerManager.RegisterColliderFunction(EnumDefine.ColliderType.PreSolve,function(contact) 
         xpcall(self.PreSolveHandle,__TRACKBACK__,self,contact)
    end)
    sol.ColliderListenerManager.RegisterColliderFunction(EnumDefine.ColliderType.PostSolve,function(contact) 
        xpcall(self.PostSolveHandle,__TRACKBACK__,self,contact) 
    end)  
   self.TriggerProxy = self:getFacade():retrieveProxy(ProxyTable.TriggerProxy)   
end 
function ColliderProxy:BeginContactHandle(contact)  
    self.TriggerProxy:TriggerBeginColliderHanlder(contact) 
end
function ColliderProxy:EndContactHandle(contact)  
end
function ColliderProxy:PreSolveHandle(contact)  
end 
function ColliderProxy:PostSolveHandle(contact) 
end  

--对象被销毁的时候要调用的函数
function ColliderProxy:ClearData()
    sol.ColliderListenerManager.UnregisterColliderFunction(EnumDefine.ColliderType.BeginContact)
    sol.ColliderListenerManager.UnregisterColliderFunction(EnumDefine.ColliderType.EndContact)
    sol.ColliderListenerManager.UnregisterColliderFunction(EnumDefine.ColliderType.PreSolve)
    sol.ColliderListenerManager.UnregisterColliderFunction(EnumDefine.ColliderType.PostSolve) 
end 
return  ColliderProxy
    
 