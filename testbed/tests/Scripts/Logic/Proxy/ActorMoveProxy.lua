local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorMoveProxy = class("ActorMoveProxy",BaseProxy)   
function ActorMoveProxy:InitData()  
    self.ActorMoveMap = {} 
end  
 
function ActorMoveProxy:LoadFinish() 
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy)  
end     

--�������Ľ���ǣ�������ƶ����ͻ���ÿ�������ҷ���һ���ƶ�״̬ˢ�¡�
function ActorMoveProxy:FlushActorMoveStatus(id,dir,time) --���ý�ɫ���ƶ�λ��  
    --�����жϵ�ǰ�Ƿ��Ѿ�ӵ���ƶ���ɫ����Ϣ
    if not self.ActorMoveMap[id] then 
        self.ActorMoveMap[id] = {} 
        self.ActorMoveMap[id].TimeObj = sol.LuaTimeEvecnt:new(self.ActorMoveMap[id])  
        self.ActorMoveMap[id].MoveDir = sol.b2Vec2.new(0,0)--���������ƶ������λ��
        self.ActorMoveMap[id].MoveCondition = EnumDefine.MoveBehavior.Reday 
        self.ActorMoveMap[id].Execute = function(table)    
            self:ClearActorMoveStatus(id) 
        end--һ���ֹͣ�ƶ�  
    end    
    self.ActorMoveMap[id].MoveCondition = EnumDefine.MoveBehavior.Reday 
    self.ActorMoveMap[id].MoveDir.x = dir.x / time
    self.ActorMoveMap[id].MoveDir.y = dir.y / time 
    sol.TimeWheelMan.Instance():AddTime(self.ActorMoveMap[id].TimeObj,time)--��ʱ�������,ִ�� 
    self.ActorFacadeProxy:GetActor(id):SetActionBits(EnumDefine.PlayerActionBits.MOVE,EnumDefine.BitsStatus.Enable)--�����ƶ�
end
function ActorMoveProxy:ClearActorMoveStatus(id,isMove) --���ý�ɫ���ƶ�λ�� 
    --�����жϵ�ǰ�Ƿ��Ѿ�ӵ���ƶ���ɫ����Ϣ
    if not self.ActorMoveMap[id] then  
        self.ActorMoveMap[id] = {} 
    end   
    self.ActorMoveMap[id].MoveCondition = EnumDefine.MoveBehavior.Over
    self.ActorMoveMap[id].TimeObj:cancel() --�����ǰ�Ķ��� 
    self.ActorFacadeProxy:GetActor(id):SetActionBits(EnumDefine.PlayerActionBits.MOVE,EnumDefine.BitsStatus.Disable) 
end  

function ActorMoveProxy:ActorMove(id) --���ý�ɫ���ƶ�λ��   
    local ActorMoveTable = self:GetActorMoveStatus(id)
    self:BodyMove(id,self.ActorMoveMap[id].MoveDir)
    if ActorMoveTable.MoveCondition ~= EnumDefine.MoveBehavior.Reday then 
        return ActorMoveTable.MoveCondition
    end  
    ActorMoveTable.MoveCondition = EnumDefine.MoveBehavior.Running
    return ActorMoveTable.MoveCondition
end 
function ActorMoveProxy:BodyMove(id,dir) 
    local actor = self.ActorFacadeProxy:GetActor(id)
    local body = actor:GetBody() 
    body:SetAwake(true)
    body:SetLinearDamping(0);
    local linearVelocity = body:GetLinearVelocity()
    local finalForce = sol.b2Vec2.new(0,0)
    finalForce.x = linearVelocity.x + InsertLine(linearVelocity.x,dir.x,0.34)--��ǰ�����ٶ� -> �������ٶ� 
    finalForce.y = linearVelocity.y + InsertLine(linearVelocity.y,dir.y,0.34)--��ǰ�����ٶ� -> �������ٶ� y 
    --local impulse = sol.b2Vec2.new((finalForce.x * body:GetMass() ) ,(finalForce.y * body:GetMass()) )
    --print(finalForce.x,finalForce.y,body:GetLinearVelocity().x,body:GetLinearVelocity().y)
    --body:ApplyLinearImpulseToCenter(impulse,true); 
    --�����湫ʽ�����ͬ�������������ʡЧ��
    body:SetLinearVelocity(finalForce); 
end 

function ActorMoveProxy:GetActorMoveStatus(id) --���ý�ɫ���ƶ�λ��  
    if not self.ActorMoveMap[id] then  
        self:ClearActorMoveStatus(id) --û�еĻ����͵������������һ���½�ɫ״̬ 
    end  
    return self.ActorMoveMap[id] 
end
return ActorMoveProxy 