local InitWorldMediator = class("InitWorldProxy") 
function InitWorldMediator:ctor()
    self.MediatorPathTable = {} 
    self.MediatorTable = {} 
    self.BasePath = "Logic.Mediator." 

    self:InitMediatorTable()
end 
function InitWorldMediator:InitMediatorTable() 
    self:RegisterMediator("ActorMoveMediator")--碰撞管理 
    self:RegisterMediator("PlayerCreateMediator")--玩家创建监听 
end 

function InitWorldMediator:RegisterMediator(path)  
    table.insert( self.MediatorTable,self.BasePath .. path ) 
end 
function InitWorldMediator:InitMediator()
    local mediatorList = {} 
    for index,path in pairs(self.MediatorTable) do
        local mediatorBase = require(path).new() 
        G_FACADE:registerMediator(mediatorBase)
        table.insert( mediatorList,mediatorBase )
    end 
    for index,mediator in pairs(mediatorList) do
        mediator:LoadFinish()
    end  
end 
return InitWorldMediator   