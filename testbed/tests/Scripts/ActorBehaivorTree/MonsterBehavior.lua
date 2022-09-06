local BehaivorFactor = require("ActorBehaivorTree.BehaivorFactor")
local MonsterBehavior = class("MonsterBehavior",BehaivorFactor)    
local MonsterBehaviorXMLDefine = [[
    <root main_tree_to_execute = "MainTree" >  
    <BehaviorTree ID="MainTree"> 
        <Sequence>  
            <Fallback> 
                <SubTree ID="MonsterTracePlayerTree"/>
                <SubTree ID="MonsterIDLETree"/>
            </Fallback>
            <SubTree ID="MonsterMoveTree"/> 
        </Sequence>
    </BehaviorTree>  

    <BehaviorTree ID="MonsterTracePlayerTree">
        <Sequence>  
            <CheckGroundPlayer TracePlayerID="PlayerID"/>
            <RectifyTraceDir Target="{PlayerID}"/> 
        </Sequence>
    </BehaviorTree>  

    <BehaviorTree ID="MonsterIDLETree">
        <ReactiveSequence> 
            <CheckIDLE/> 
            <Timeout msec="200">
               <MonsterIDLE/>
            </Timeout>  
        </ReactiveSequence>
    </BehaviorTree> 
    
    <BehaviorTree ID="MonsterMoveTree">
        <Sequence> 
            <CheckMove/>
            <MonsterMOVE/> 
        </Sequence>
    </BehaviorTree>    
    </root> 
]]
function MonsterBehavior:ctor() 
    BehaivorFactor.ctor(self,MonsterBehaviorXMLDefine) 
end   


function CheckIDLE(node,paramTable)  
    local Actor = paramTable.Actor  
    local existAction = Actor:GetActionBits(EnumDefine.PlayerActionBits.ALL) == 0  --当前没有任何多余的动作  
    return existAction and EnumDefine.NodeStatus.SUCCESS or EnumDefine.NodeStatus.FAILURE
end 

function MonsterIDLE(node,paramTable)--空闲状态下 
    local Actor = paramTable.Actor 
    ThreadRunning(function()  
        return true
    end )--永远为运行态
    return EnumDefine.NodeStatus.SUCCESS
end 

function CheckMove(node,paramTable)    
    local Actor = paramTable.Actor 
    local existAction = Actor:GetActionBits( EnumDefine.PlayerActionBits.MOVE ) == 1  --可以移动
    return existAction and EnumDefine.NodeStatus.SUCCESS or EnumDefine.NodeStatus.FAILURE
end 

function MonsterMOVE(node,paramTable)  
    local Actor = paramTable.Actor 
    local ActorMoveProxy =  G_FACADE:retrieveProxy(ProxyTable.ActorMoveProxy)    
    local actorID = Actor:GetID()
     --返回 运行中，代表当前还在移动，返回成功，代表当前已经移动完成了 
     ThreadRunning(function()  
        return ActorMoveProxy:ActorMove(actorID) == EnumDefine.MoveBehavior.Running
    end )  --持续到条件不满足的时候 
    return EnumDefine.NodeStatus.SUCCESS 
end

function CheckGroundPlayer(node,paramTable)   
    local Actor = paramTable.Actor
    local actorID = Actor:GetID("TracePlayerID")
    --local visbleHeroMap = sol.DistanceMan:Instance():GetViewBody(actorID,EnumDefine.ViewType.Monster_Hero,EnumDefine.ViewStatus.Visble)
    ----找到第一个 开始追踪
    --if not visbleHeroMap then 
    --    return EnumDefine.NodeStatus.FAILURE 
    --end   
    --for v,k in pairs(visbleHeroMap) do  
        node:SetOutput("TracePlayerID",1) 
    --    break
    --end  
    return EnumDefine.NodeStatus.SUCCESS 
end

function RectifyTraceDir(node,paramTable)   
    local Actor = paramTable.Actor
    local actorID = Actor:GetID("TracePlayerID")

    local targetId = tonumber(node:GetInput("Target"))

    local ActorFacadeProxy =G_FACADE:retrieveProxy(ProxyTable.ActorFacadeProxy) --角色移动代理
    local ActorMoveProxy =G_FACADE:retrieveProxy(ProxyTable.ActorMoveProxy) --角色移动代理 
    local targetActor = ActorFacadeProxy:GetActor(targetId)
    local dir = sol.b2Vec2.new(targetActor:GetBody():GetPosition().x ,targetActor:GetBody():GetPosition().y) 
    dir.x = dir.x - Actor:GetBody():GetPosition().x
    dir.y = dir.y - Actor:GetBody():GetPosition().y 
    dir:Normalize()
    dir.x = dir.x / 3
    dir.y = dir.y / 3 
    ActorMoveProxy:FlushActorMoveStatus(actorID, dir,0.5) 
    return EnumDefine.NodeStatus.SUCCESS 
end

function MonsterBehavior:BehaviorInit()
    self:RegisterConditionAction("CheckMove", CheckMove);
    self:RegisterConditionAction("CheckGroundPlayer", CheckGroundPlayer,{"TracePlayerID"}); 
    self:RegisterConditionAction("RectifyTraceDir", RectifyTraceDir,{"Target"}); 
    self:RegisterCoroAction("CheckIDLE", CheckIDLE);
    self:RegisterCoroAction("MonsterIDLE", MonsterIDLE); 
    self:RegisterCoroAction("MonsterMOVE", MonsterMOVE); 
end     
return MonsterBehavior 
  

