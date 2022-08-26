local BehaivorFactor = require("ActorBehaivorTree.BehaivorFactor")
local PlayerBehavior = class("PlayerBehavior",BehaivorFactor)   
local PlayerBehaviorXMLDefine = [[  
    <root main_tree_to_execute = "MainTree" >  
    <BehaviorTree ID="MainTree"> 
        <Fallback> 
            <SubTree ID="PlayerIDLETree"/>
            <SubTree ID="PlayerMoveTree"/>
        </Fallback>
    </BehaviorTree>  

    <BehaviorTree ID="PlayerIDLETree">
        <ReactiveSequence> 
            <CheckIDLE/>
            <PlayerIDLE/> 
        </ReactiveSequence>
    </BehaviorTree> 
    <BehaviorTree ID="PlayerMoveTree">
        <ReactiveSequence> 
            <CheckMove/>
            <PlayerMOVE/> 
        </ReactiveSequence>
    </BehaviorTree>    
    </root> 
]]  
--一个玩家拥有的状态
--站立 行走 闪现 闪避 跳跃 技能
--一个玩家拥有的buff状态
--眩晕 击飞 沉默 缚地 

--一个技能的动作有三个阶段
--技能不可以同时释放
--但是技能分为三个阶段 前摇 施法 后摇
--在特殊情况下，如果前摇 后摇 的操作时间很短，是可以做到瞬间施法
--拿老鼠的技能作比喻

--技能需要一个技能状态机
--状态机不常在内存中，如果玩家队列中不存在技能信息，技能状态机就会进入休眠状态 直到下一次被释放技能唤醒
--可以考虑把每一个技能划分为一颗设备树，用于管理技能的释放
--拿老鼠的技能作比喻
--玩家释放Q技能，Q技能创建一个行为树，加入到玩家技能队列之中。 
--技能管理在玩家对列下添加这个行为树
--技能树，会在下一帧检查玩家是否可以释放技能，譬如玩家被沉默一类，检查完成后，会给玩家添加一系列的行为Buff（无法移动，锁定技能（只能释放当前技能）等）
--如果可以，技能树会进入循环判断前摇的阶段，判断前摇阶段时，会确定有没有被对方施法打断，如果没有判断当前技能是否有前摇，如果前摇时间为0，那么直接进入施法阶段
--施法阶段时一个循环的判断阶段，判断有没有可以的去打断技能（譬如行走，如果没有进入施法阶段）
--技能会检查当前技能的运行时间，如果为瞬发 释放完成后，直接去到后摇阶段
--如果为多时延阶段，每一阶段进行计时，到达后，进行释放。直到施法阶段完全结束后，进入后摇阶段
--后摇阶段与前摇阶段一样，会检查是否有动作中止了后摇，譬如移动，释放了一个新技能等
--进入后摇执行阶段
--一个完整的技能流程结束
function PlayerBehavior:ctor() 
    BehaivorFactor.ctor(self,PlayerBehaviorXMLDefine) 
end 
 
function CheckIDLE(node,paramTable)  
    local Actor = paramTable.Actor  
    local existAction = Actor:GetActionBits(EnumDefine.PlayerActionBits.ALL) == 0  --当前没有任何多余的动作  
    return existAction and EnumDefine.NodeStatus.SUCCESS or EnumDefine.NodeStatus.FAILURE
end 
function PlayerIDLE(node,paramTable)--空闲状态下，会循环的检测当前的速度是否是0，如果不是会循环的设置当前的速度
    local Actor = paramTable.Actor  
    local Body = Actor:GetBody()
    --获取到当前的线速度
    local LineVelocity = Body:GetLinearVelocity()
    local WorldGravity = Body:GetWorldGravity()
    if LineVelocity:Length() >= 0.1 then 
        --print("AAAAAAAAAAAAA",WorldGravity.x,WorldGravity.y,LineVelocity.y,WorldGravity.y * (1/WorldGravity.y))
        --if (WorldGravity.x == 0 and (LineVelocity.y * WorldGravity.y * (-1/WorldGravity.y))<= 0.3  or (WorldGravity.y == 0 and  WorldGravity.y * (-1/WorldGravity.y) * LineVelocity.x) <= 0.3 ) then 
        --    print("BBBBBBBBBBBBB")
        --    G_FACADE:sendNotification(NotifyDefine.KeyInput,{pushKey = ' '})  
        --end  
    end 
    return EnumDefine.NodeStatus.SUCCESS
end 
function CheckMove(node,paramTable)  
    local Actor = paramTable.Actor  
    local existAction = Actor:GetActionBits( EnumDefine.PlayerActionBits.MOVE ) == 1   
    return existAction and EnumDefine.NodeStatus.SUCCESS or EnumDefine.NodeStatus.FAILURE
end 
function PlayerMOVE(node,paramTable)  
    local Actor = paramTable.Actor  
    local ActorMoveProxy =  G_FACADE:retrieveProxy(ProxyTable.ActorMoveProxy)    
    local actorID = Actor:GetID()
     --返回 运行中，代表当前还在移动，返回成功，代表当前已经移动完成了 
    ThreadRunning(function() 
        return ActorMoveProxy:ActorMove(actorID) == EnumDefine.MoveBehavior.Running
    end )  --持续到条件不满足的时候 
    return EnumDefine.NodeStatus.SUCCESS 
end

function PlayerBehavior:BehaviorInit()
    self:RegisterConditionAction("CheckIDLE", CheckIDLE); --初始化sayMsg方法  
    self:RegisterConditionAction("CheckMove", CheckMove); --初始化sayMsg方法   
    self:RegisterCoroAction("PlayerIDLE", PlayerIDLE); --初始化sayMsg方法  
    self:RegisterCoroAction("PlayerMOVE", PlayerMOVE); --初始化sayMsg方法  
end     
return PlayerBehavior 