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
--��ͼ����ᴴ��һ�����磬Ȼ������������ӽ������ ������ĺ���ά�� 
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
--���ٶ��󴥷�����
function TriggerProxy:DestoryTrigger(actor,time) 
    local actorID = actor:GetID()
    --Ŀǰ�����п����Զ��ͷ�
    --self.SimpleTimeWheelProxy:AddTimeHandle(time,function(time)   
    --   self.ActorFacadeProxy:DestoryActor(actorID)
    --end)
end
function TriggerProxy:TransfromTrigger(actor,x,y,time) 
    --self.SimpleTimeWheelProxy:AddTimeHandle(time,function(time)   
    --    actor:SetPosition(x,y)
    --end ) 
end
--���ٶ��󴥷�����
function TriggerProxy:ColliderTransfromTrigger(contact,data,isMain)
    local x = assert(tonumber(data.param1),"�����X���겻��ȷ")
    local y = assert(tonumber(data.param2),"�����Y���겻��ȷ")
    local time = assert(tonumber(data.param3),"����ĳ�ʱ�¼�����ȷ") 
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID()) 
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID()) 
    local MainObj = isMain and ActorA or ActorB   
    self:TransfromTrigger(MainObj,x,y,time); 
end
--�����ײ����ʹ�ã�����������ײ�Ķ���
function TriggerProxy:ColliderDestoryTrigger(contact,data,isMain)
    --����(1:�Լ� 2�Է� 3:��Ұ�����Ŀ�� 4:��Ұ��Զ��Ŀ��)
    --����һ��Բ �� ��һ��Բ���������
    local obj = assert(tonumber(data.param1),"����Ĳ�������ȷ") --���ٶ���
    local time = assert(tonumber(data.param2),"����Ĳ�������ȷ") --�����¼� 
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
--�����ײ����ʹ�ã�����������ײ�Ķ���
function TriggerProxy:ColliderAttackTrigger(contact,data,isMain) 
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID())
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID()) 
    local MainObj = isMain and ActorA or ActorB   
    local MinObj = ( not isMain and ActorA )or ActorB  
    local attack = MainObj:GetAttr(EnumDefine.AttributeKey.Attack)--��ȡ��A�Ĺ�����
    local core = MinObj:GetCoreAttr() --��ȡ��B��Ѫ�������Ѫ��
    --print(attack,"������Ĺ��� ",core.HP,core.MP,core.Exp,"��ǰ������")  
    core.HP = core.HP - attack
    --print("ǽ�幥������ң������ " .. attack .. "���˺�")
    --print("���ʣ��Ѫ�� "..core.HP)
    if core.HP <= 0 then 
        print("���������ǽ���ܵ���Ӧ�еĳͷ��������٣�") 
        self:DestoryTrigger(MainObj,0);
    end 
end

--�����ײ����ʹ�ã������������ߵ���ײ����
function TriggerProxy:ColliderElassticityTrigger(contact,data)
    local elassticity = assert(tonumber(data.param1),"����Ĳ�������ȷ") 
    contact:SetRestitution(elassticity)
end
--�����ײ����ʹ�ã������������ߵ�Ħ������
function TriggerProxy:ColliderFrictionTrigger(contact,data) 
    local friction = assert(tonumber(data.param1),"����Ĳ�������ȷ")--����1Ϊ �����ٶ�   
    contact:SetFriction(friction);
end
--���һ���ƶ�
function TriggerProxy:ColliderTangentTrigger(contact,data)  
    local speed = assert(tonumber(data.param1),"����Ĳ�������ȷ") --����1Ϊ �����ٶ�   
    contact:SetTangentSpeed(speed)
end
--���һ���ƶ�
function TriggerProxy:Trigger_MonsterGenerate(actor,data)  
    modelID = assert(tonumber(data.param1),"�����ģ��ID����ȷ") 
    actorID = assert(tonumber(data.param2),"����Ľ�ɫID����ȷ")   
    self.ActorFacadeProxy:CreateActor(modelID,actorID,actor:GetBody():GetPosition(),1)   
end 

--ִ�д���
function TriggerProxy:ExecuteTriggerCollider(triggerList,contact,isMain)  
    for v,k in pairs(triggerList) do
        if self.TriggerTable[v] then  
           self.TriggerTable[v](contact,k,isMain) 
        end
    end 
end 
--��ײ�������� 
function TriggerProxy:TriggerBeginColliderHanlder(contact)   
    local ActorA = self.ActorFacadeProxy:GetActor(contact:GetBodyA():ID())
    local ActorB = self.ActorFacadeProxy:GetActor(contact:GetBodyB():ID())   
    local BeginContactActionA = ActorA:GetColliderConfig(EnumDefine.ColliderType.BeginContact ) 
    local BeginContactActionB = ActorB:GetColliderConfig(EnumDefine.ColliderType.BeginContact )   
    local _ = BeginContactActionA and self:ExecuteTriggerCollider(BeginContactActionA,contact,true) 
    local _ = BeginContactActionB and self:ExecuteTriggerCollider(BeginContactActionB,contact,false)    
end 

--ִ�д���
function TriggerProxy:ExecuteTrigger(actor,triggerList) 
    for v,k in pairs(triggerList) do   
        if self.TriggerTable[v] then  
           self.TriggerTable[v](actor,k) 
        end
    end 
end
return TriggerProxy 