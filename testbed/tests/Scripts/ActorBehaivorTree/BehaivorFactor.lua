local BehaivorBase = class("BehaivorBase")
local CoroActionNode = require("ActorBehaivorTree.CoroActionNode")
function BehaivorBase:ctor(xml)  
    self:__InitData(xml) 
end 
--�������ݵĳ�ʼ��
function BehaivorBase:__InitData(xml) 
    self.BehaviorFactor = sol.BTFactory.new() --����һ������ 
    self:BehaviorInit() --��ʼ����Ϊ��
    self.BehaviorFactor:registerBehaviorTreeFromText(xml)
end   
function BehaivorBase:RegisterCoroAction(id,handle,port)
    local safeFunc = function(...) 
        local status,result = xpcall(handle, __TRACKBACK__,...)   
        return result
    end  
    local CoroObj  = CoroActionNode.new(safeFunc) 
    self.BehaviorFactor:RegisterCoroAction(id, CoroObj,port or {} );
end  

function BehaivorBase:RegisterConditionAction(id,handle,port)
    local safeFunc = function(...)  
        local status,result = xpcall(handle, __TRACKBACK__,...)   
        return result
    end  
    self.BehaviorFactor:RegisterConditionAction(id,safeFunc,port or {} ); --��ʼ��sayMsg���� 
end

--��ʼ���ڲ�����
function BehaivorBase:BehaviorInit()
end    

function BehaivorBase:CreateTree(param) 
    local tree = self.BehaviorFactor:createTree()
    tree:InitLuaNodeParam(param)
    return tree
end 
return BehaivorBase  