
local BaseMediator = require("Logic.Mediator.BaseMediator")
local ActorOperationMediator = class("ActorOperationMediator",BaseMediator)   
function ActorOperationMediator:InitData() 
    self.ActorMoveProxy =G_FACADE:retrieveProxy(ProxyTable.ActorMoveProxy) --��ɫ�ƶ�����
    self.ActorBehaivorProxy =G_FACADE:retrieveProxy(ProxyTable.ActorBehaivorProxy) --��ɫ�ƶ�����
    self.ActorFacadeProxy =G_FACADE:retrieveProxy(ProxyTable.ActorFacadeProxy) --��ɫ�ƶ�����
end   
function ActorOperationMediator:KeyInput_Handle(data)
    local move = {}
    move['W'] = sol.b2Vec2.new(0,1)
    move['S'] = sol.b2Vec2.new(0,-1)
    move['A'] = sol.b2Vec2.new(-1,0)
    move['D'] = sol.b2Vec2.new(1,0)
    move['Q'] = sol.b2Vec2.new(-1,1)
    move['E'] = sol.b2Vec2.new(1,1)
    move['Z'] = sol.b2Vec2.new(-1,-1)
    move['C'] = sol.b2Vec2.new(1,-1)    
    move[' '] = sol.b2Vec2.new(0,0)    
 
    print(data.actorID,"��ǰ�Ľ�ɫID")
    if move[data.pushKey] then 
        move[data.pushKey].x = move[data.pushKey].x / 5
        move[data.pushKey].y = move[data.pushKey].y /5
        self.ActorMoveProxy:FlushActorMoveStatus(data.actorID, move[data.pushKey],1)
        --self.NavmeshManagerProxy = self:getFacade():retrieveProxy(ProxyTable.NavmeshManagerProxy) 
        --print("��ǰ�ľ���Ϊ",self.NavmeshManagerProxy:Recast(Actor,sol.b2Vec2.new(30,30)) )
    end  
    print(self.ActorBehaivorProxy.Count,"��ǰ��Ϊ������")
end

function ActorOperationMediator:NotifycationRegister(table) 
    table[NotifyDefine.KeyInput] = handler(self,self.KeyInput_Handle)
end 

--�������ٵ�ʱ��Ҫ���õĺ���
function ActorOperationMediator:ClearData() 
end 
return  ActorOperationMediator 