local BaseProxy = require("Logic.Proxy.BaseProxy")
local ActorMoveProxy = class("ActorMoveProxy",BaseProxy)   
function ActorMoveProxy:InitData()  
    self.ActorMoveMap = {} 
end  
 
function ActorMoveProxy:LoadFinish() 
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy)  
end     

--我期望的结果是，如果向移动，客户端每秒钟像我发送一次移动状态刷新。
function ActorMoveProxy:FlushActorMoveStatus(id,dir,time) --设置角色的移动位置  
    --首先判断当前是否已经拥有移动角色的消息
    if not self.ActorMoveMap[id] then 
        self.ActorMoveMap[id] = {} 
        self.ActorMoveMap[id].TimeObj = sol.LuaTimeEvecnt:new(self.ActorMoveMap[id])  
        self.ActorMoveMap[id].MoveDir = sol.b2Vec2.new(0,0)--在五秒钟移动到这个位置
        self.ActorMoveMap[id].MoveCondition = EnumDefine.MoveBehavior.Reday 
        self.ActorMoveMap[id].Execute = function(table)    
            self:ClearActorMoveStatus(id) 
        end--一秒后停止移动  
    end    
    self.ActorMoveMap[id].MoveCondition = EnumDefine.MoveBehavior.Reday 
    self.ActorMoveMap[id].MoveDir.x = dir.x / time
    self.ActorMoveMap[id].MoveDir.y = dir.y / time 
    sol.TimeWheelMan.Instance():AddTime(self.ActorMoveMap[id].TimeObj,time)--延时多少秒后,执行 
    self.ActorFacadeProxy:GetActor(id):SetActionBits(EnumDefine.PlayerActionBits.MOVE,EnumDefine.BitsStatus.Enable)--是能移动
end
function ActorMoveProxy:ClearActorMoveStatus(id,isMove) --设置角色的移动位置 
    --首先判断当前是否已经拥有移动角色的消息
    if not self.ActorMoveMap[id] then  
        self.ActorMoveMap[id] = {} 
    end   
    self.ActorMoveMap[id].MoveCondition = EnumDefine.MoveBehavior.Over
    self.ActorMoveMap[id].TimeObj:cancel() --清楚当前的动作 
    self.ActorFacadeProxy:GetActor(id):SetActionBits(EnumDefine.PlayerActionBits.MOVE,EnumDefine.BitsStatus.Disable) 
end  

function ActorMoveProxy:ActorMove(id) --设置角色的移动位置   
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
    finalForce.x = linearVelocity.x + InsertLine(linearVelocity.x,dir.x,0.34)--当前的线速度 -> 期望的速度 
    finalForce.y = linearVelocity.y + InsertLine(linearVelocity.y,dir.y,0.34)--当前的线速度 -> 期望的速度 y 
    --local impulse = sol.b2Vec2.new((finalForce.x * body:GetMass() ) ,(finalForce.y * body:GetMass()) )
    --print(finalForce.x,finalForce.y,body:GetLinearVelocity().x,body:GetLinearVelocity().y)
    --body:ApplyLinearImpulseToCenter(impulse,true); 
    --与上面公式结果相同，下面这个更加省效率
    body:SetLinearVelocity(finalForce); 
end 

function ActorMoveProxy:GetActorMoveStatus(id) --设置角色的移动位置  
    if not self.ActorMoveMap[id] then  
        self:ClearActorMoveStatus(id) --没有的话，就调用清理，清理出一个新角色状态 
    end  
    return self.ActorMoveMap[id] 
end
return ActorMoveProxy 