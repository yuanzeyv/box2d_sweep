--游戏角色中 的所有系统都会向本模块中注册，本模块中保存着角色的所有属性 
--一个角色身上有属性，有碰撞，有定时器
local ActorBase = class("ActorBase")  
function ActorBase:ctor(...)
    self:InitData()
end 
function ActorBase:Destory()
    self:CloseTimer()--关闭所有定时器
    sol.BodyMan.Instance():DeleteBody(self.ID)--删除自己 
end 
function ActorBase:InitData()  --将身体信息加入到角色  
    self.BodyData = nil 
    self.LevelTable = nil 
    self.ID = nil 
    self.ActorType = nil
    self.Level = nil
    self.CoreAttr = {}  
    self.TimerList = {} --默认会给我分配一个 定时函数，让我运行  
    self.ActorFacadeProxy = G_FACADE:retrieveProxy(ProxyTable.ActorFacadeProxy)  
    self:CoreAttrSetZero()

    self.ActionBits = 0
end

function ActorBase:CoreAttrSetZero()
    self.CoreAttr.Exp = 0
    self.CoreAttr.HP = 0
    self.CoreAttr.MP =0 
end 

function ActorBase:ReCalcCoreAttr() 
    local MaxHP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.HP)
    local MaxMP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.MP) 
    local MaxExp = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.UpLevelExp) 
    self.CoreAttr.HP = (self.CoreAttr.HP > MaxHP) and MaxHP or self.CoreAttr.HP;
    self.CoreAttr.MP = (self.CoreAttr.MP > MaxMP) and MaxMP or self.CoreAttr.MP;
    self.CoreAttr.Exp = (self.CoreAttr.HP > MaxExp) and MaxExp or self.CoreAttr.Exp;  
end 

function ActorBase:InitCoreAttr()
    self.CoreAttr.Exp = 0
    self.CoreAttr.HP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.HP)
    self.CoreAttr.MP = self.ActorFacadeProxy:GetAttrValue(self.ID,EnumDefine.AttributeKey.MP)
end 
function ActorBase:GetCoreAttr()
    return self.CoreAttr
end  

function ActorBase:SetLevelTable(levelTable)
    self.LevelTable = levelTable
end  

function ActorBase:GetLevelConfig()
    return self.LevelTable[self.Level]
end  

function ActorBase:SetLevel(level)
    self.Level = level
end
function ActorBase:GetLevel()
    return self.Level
end   
--获取到Body
function ActorBase:GetBody()
    return self.Body
end  
function ActorBase:SetBody(body)
    self.BodyData = body
    self.Body = body.Body
    self.ID = body.ID
end 

--获取到ID
function ActorBase:GetID()
    return self.ID
end 

function ActorBase:SetTimer(timeList)
    self.TimerList = timeList
end 

function ActorBase:OpenTimer(index) 
    index = (index and index) or 0
    local OperList = {[0] = {1,2,3,},[1] = {1} ,[2]={2},[3]={3}}   
    for v,k in pairs(OperList) do 
        if self.TimerList[v] then  
            self.TimerList[v]:OpenTimer() 
        end
    end  
end 
function ActorBase:CloseTimer(index)
    index = (index and index) or 0
    local OperList = {[0] = {1,2,3,},[1] = {1} ,[2]={2},[3]={3}}    
    for v,k in pairs(OperList[index] or {} ) do 
        if self.TimerList[v] then  
            self.TimerList[v]:CloseTimer() 
        end  
    end  
end  
   
--获取到碰撞信息
function ActorBase:GetColliderConfig(colliderType)
    local mappingTable = {} 
    mappingTable[EnumDefine.ColliderType.BeginContact] = "BeginCollider"
    mappingTable[EnumDefine.ColliderType.EndContact] = "EndCollider" 
    mappingTable[EnumDefine.ColliderType.PostSolve] = "ContinueCollider"  
    return self.BodyData.ModelTriggerTable[mappingTable[colliderType]]
end 
--等级提升
function ActorBase:LevelUP()
    --self.LevelTable[] 对角色的等级进行提升
end 
 
--获取到当前的属性
function ActorBase:GetAttr(type)
    return self.ActorFacadeProxy:GetAttrValue(self.ID,type)
end  

--获取到当前的角色动作属性
function ActorBase:GetActionBits(bits)
    bits = assert(tonumber(bits),"当前要查询的字节不是整数") 
    return self.ActionBits & bits --返回要查询的字段
end 

--获取到当前的角色动作属性
function ActorBase:SetActionBits(offset,value)  
    if value == 0 then  
        self.ActionBits = self.ActionBits ~ offset  
    else 
        self.ActionBits = self.ActionBits | offset  
    end   
end   

--设置角色类型
function ActorBase:SetActorType(type) --角色类型
    self.ActorType = type
end 
function ActorBase:GetActorType(type) --角色类型
    return self.ActorType
end 
--获取角色类型
return ActorBase