local BaseProxy = require("Logic.Proxy.BaseProxy")
local TriggerProxy = class("TriggerProxy",BaseProxy) 
local EnumTriggerType = {
    Trigger_Collider_Destory = 1,
    Trigger_Collider_Elasticity = 2,
    Trigger_Collider_Friction = 3,  
    Trigger_Collider_Boom = 4,  
    Trigger_Collider_Tangent_Move = 5,
    Trigger_Collider_Intercept = 6,
    Trigger_Collider_Transfer = 7,
    Trigger_Destory = 8,
    Trigger_Boom = 9,
    Trigger_Transfer = 10,
    Trigger_MonsterGenerate = 11,
    Trigger_Collider_Attack = 13,
}
--地图代理会创建一个世界，然后再世界上添加建筑物，和 对世界的后期维护 
function TriggerProxy:InitData() 
    self.TriggerTable = {}
 end 
function TriggerProxy:LoadFinish()  
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
    self.MapProxy = self:getFacade():retrieveProxy(ProxyTable.MapProxy)  
    self:InitTriggerTable()
end 
function TriggerProxy:InitTriggerTable()
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Destory] = handler(self,TriggerProxy.ColliderDestoryTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Elasticity] = handler(self,TriggerProxy.ColliderElassticityTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Friction] = handler(self,TriggerProxy.ColliderFrictionTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Boom] = handler(self,function() end );
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Tangent_Move] = handler(self,TriggerProxy.ColliderTangentTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Intercept] = handler(self,function() end );
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Transfer] = handler(self,TriggerProxy.ColliderTransfromTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_Destory] = handler(self,TriggerProxy.DestoryTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_Boom] = handler(self,function() end );
    self.TriggerTable[EnumTriggerType.Trigger_Transfer] = handler(self,TriggerProxy.TransfromTrigger) 
    self.TriggerTable[EnumTriggerType.Trigger_MonsterGenerate] = handler(self,TriggerProxy.Trigger_MonsterGenerate) 
    self.TriggerTable[EnumTriggerType.Trigger_Collider_Attack] = handler(self,TriggerProxy.ColliderAttackTrigger) 
end 
--销毁对象触发函数
function TriggerProxy:DestoryTrigger(actor,time) 
    local actorID = actor:GetID()
    --目前这里有可能自动释放
    --self.SimpleTimeWheelProxy:AddTimeHandle(time,function(time)   
    --   self.ActorFacadeProxy:DestoryActor(actorID)
    --end)
end
function TriggerProxy:TransfromTrigger(actor,x,y,time) 
    --self.SimpleTimeWheelProxy:AddTimeHandle(time,function(time)   
    --    actor:SetPosition(x,y)
    --end ) 
end
--销毁对象触发函数
function TriggerProxy:ColliderTransfromTrigger(contact,data,isMain)
    local x = assert(tonumber(data.param1),"传入的X坐标不正确")
    local y = assert(tonumber(data.param2),"传入的Y坐标不正确")
    local time = assert(tonumber(data.param3),"传入的超时事件不正确") 
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID()) 
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID()) 
    local MainObj = isMain and ActorA or ActorB   
    self:TransfromTrigger(MainObj,x,y,time); 
end
--针对碰撞函数使用，用于销毁碰撞的对象
function TriggerProxy:ColliderDestoryTrigger(contact,data,isMain)
    --对象(1:自己 2对方 3:视野最近的目标 4:视野最远的目标)
    --计算一个圆 到 另一个圆的最近距离
    local obj = assert(tonumber(data.param1),"传入的参数不正确") --销毁对象
    local time = assert(tonumber(data.param2),"传入的参数不正确") --销毁事件 
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID()) 
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID()) 
    local MainObj = isMain and ActorA or ActorB   
    local MinObj = ( not isMain and ActorA )or ActorB 
    local operObj = MainObj
    if data.param1 == 2 then 
        operObj = MinObj
    end  
    self:DestoryTrigger(operObj,time);
end
--针对碰撞函数使用，用于销毁碰撞的对象
function TriggerProxy:ColliderAttackTrigger(contact,data,isMain) 
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID())
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID()) 
    local MainObj = isMain and ActorA or ActorB   
    local MinObj = ( not isMain and ActorA )or ActorB  
    local attack = MainObj:GetAttr(EnumDefine.AttributeKey.Attack)--获取到A的攻击力
    local core = MinObj:GetCoreAttr() --获取到B的血量与最大血量
    --print(attack,"主对象的攻击 ",core.HP,core.MP,core.Exp,"当前的属性")  
    core.HP = core.HP - attack
    --print("墙体攻击了玩家，造成了 " .. attack .. "点伤害")
    --print("玩家剩余血量 "..core.HP)
    if core.HP <= 0 then 
        print("玩家死亡，墙体受到了应有的惩罚（被销毁）") 
        self:DestoryTrigger(MainObj,0);
    end 
end

--针对碰撞函数使用，用于设置两边的碰撞力度
function TriggerProxy:ColliderElassticityTrigger(contact,data)
    local elassticity = assert(tonumber(data.param1),"传入的参数不正确") 
    contact:SetRestitution(elassticity)
end
--针对碰撞函数使用，用于设置两边的摩擦力度
function TriggerProxy:ColliderFrictionTrigger(contact,data) 
    local friction = assert(tonumber(data.param1),"传入的参数不正确")--参数1为 切线速度   
    contact:SetFriction(friction);
end
--添加一个移动
function TriggerProxy:ColliderTangentTrigger(contact,data)  
    local speed = assert(tonumber(data.param1),"传入的参数不正确") --参数1为 切线速度   
    contact:SetTangentSpeed(speed)
end
--添加一个移动
function TriggerProxy:Trigger_MonsterGenerate(actor,data)  
    modelID = assert(tonumber(data.param1),"传入的模型ID不正确") 
    actorID = assert(tonumber(data.param2),"传入的角色ID不正确")   
    self.ActorFacadeProxy:CreateActor(modelID,actorID,actor:GetBody():GetPosition(),1)   
end 

--执行触发
function TriggerProxy:ExecuteTriggerCollider(triggerList,contact,isMain)  
    for v,k in pairs(triggerList) do
        if self.TriggerTable[v] then  
           self.TriggerTable[v](contact,k,isMain) 
        end
    end 
end 
--碰撞触发区域 
function TriggerProxy:TriggerBeginColliderHanlder(contact)   
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID())
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID())   
    local BeginContactActionA = ActorA:GetColliderConfig(EnumDefine.ColliderType.BeginContact ) 
    local BeginContactActionB = ActorB:GetColliderConfig(EnumDefine.ColliderType.BeginContact )   
    local _ = BeginContactActionA and self:ExecuteTriggerCollider(BeginContactActionA,contact,true) 
    local _ = BeginContactActionB and self:ExecuteTriggerCollider(BeginContactActionB,contact,false)    
end 

--执行触发
function TriggerProxy:ExecuteTrigger(actor,triggerList) 
    for v,k in pairs(triggerList) do   
        if self.TriggerTable[v] then  
           self.TriggerTable[v](actor,k) 
        end
    end 
end
return TriggerProxy 