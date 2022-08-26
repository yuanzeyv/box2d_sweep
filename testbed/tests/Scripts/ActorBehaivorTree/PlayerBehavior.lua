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
--һ�����ӵ�е�״̬
--վ�� ���� ���� ���� ��Ծ ����
--һ�����ӵ�е�buff״̬
--ѣ�� ���� ��Ĭ ���� 

--һ�����ܵĶ����������׶�
--���ܲ�����ͬʱ�ͷ�
--���Ǽ��ܷ�Ϊ�����׶� ǰҡ ʩ�� ��ҡ
--����������£����ǰҡ ��ҡ �Ĳ���ʱ��̣ܶ��ǿ�������˲��ʩ��
--������ļ���������

--������Ҫһ������״̬��
--״̬���������ڴ��У������Ҷ����в����ڼ�����Ϣ������״̬���ͻ��������״̬ ֱ����һ�α��ͷż��ܻ���
--���Կ��ǰ�ÿһ�����ܻ���Ϊһ���豸�������ڹ����ܵ��ͷ�
--������ļ���������
--����ͷ�Q���ܣ�Q���ܴ���һ����Ϊ�������뵽��Ҽ��ܶ���֮�С� 
--���ܹ�������Ҷ�������������Ϊ��
--��������������һ֡�������Ƿ�����ͷż��ܣ�Ʃ����ұ���Ĭһ�࣬�����ɺ󣬻��������һϵ�е���ΪBuff���޷��ƶ����������ܣ�ֻ���ͷŵ�ǰ���ܣ��ȣ�
--������ԣ������������ѭ���ж�ǰҡ�Ľ׶Σ��ж�ǰҡ�׶�ʱ����ȷ����û�б��Է�ʩ����ϣ����û���жϵ�ǰ�����Ƿ���ǰҡ�����ǰҡʱ��Ϊ0����ôֱ�ӽ���ʩ���׶�
--ʩ���׶�ʱһ��ѭ�����жϽ׶Σ��ж���û�п��Ե�ȥ��ϼ��ܣ�Ʃ�����ߣ����û�н���ʩ���׶Σ�
--���ܻ��鵱ǰ���ܵ�����ʱ�䣬���Ϊ˲�� �ͷ���ɺ�ֱ��ȥ����ҡ�׶�
--���Ϊ��ʱ�ӽ׶Σ�ÿһ�׶ν��м�ʱ������󣬽����ͷš�ֱ��ʩ���׶���ȫ�����󣬽����ҡ�׶�
--��ҡ�׶���ǰҡ�׶�һ���������Ƿ��ж�����ֹ�˺�ҡ��Ʃ���ƶ����ͷ���һ���¼��ܵ�
--�����ҡִ�н׶�
--һ�������ļ������̽���
function PlayerBehavior:ctor() 
    BehaivorFactor.ctor(self,PlayerBehaviorXMLDefine) 
end 
 
function CheckIDLE(node,paramTable)  
    local Actor = paramTable.Actor  
    local existAction = Actor:GetActionBits(EnumDefine.PlayerActionBits.ALL) == 0  --��ǰû���κζ���Ķ���  
    return existAction and EnumDefine.NodeStatus.SUCCESS or EnumDefine.NodeStatus.FAILURE
end 
function PlayerIDLE(node,paramTable)--����״̬�£���ѭ���ļ�⵱ǰ���ٶ��Ƿ���0��������ǻ�ѭ�������õ�ǰ���ٶ�
    local Actor = paramTable.Actor  
    local Body = Actor:GetBody()
    --��ȡ����ǰ�����ٶ�
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
     --���� �����У�����ǰ�����ƶ������سɹ�������ǰ�Ѿ��ƶ������ 
    ThreadRunning(function() 
        return ActorMoveProxy:ActorMove(actorID) == EnumDefine.MoveBehavior.Running
    end )  --�����������������ʱ�� 
    return EnumDefine.NodeStatus.SUCCESS 
end

function PlayerBehavior:BehaviorInit()
    self:RegisterConditionAction("CheckIDLE", CheckIDLE); --��ʼ��sayMsg����  
    self:RegisterConditionAction("CheckMove", CheckMove); --��ʼ��sayMsg����   
    self:RegisterCoroAction("PlayerIDLE", PlayerIDLE); --��ʼ��sayMsg����  
    self:RegisterCoroAction("PlayerMOVE", PlayerMOVE); --��ʼ��sayMsg����  
end     
return PlayerBehavior 