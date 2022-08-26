local BaseProxy = require("Logic.Proxy.BaseProxy")
local PlayerManagerProxy = class("PlayerManagerProxy",BaseProxy)   
function PlayerManagerProxy:InitData()
end  
 
function PlayerManagerProxy:LoadFinish()  
    self.NavmeshManagerProxy = self:getFacade():retrieveProxy(ProxyTable.NavmeshManagerProxy)
    self.ActorFacadeProxy = self:getFacade():retrieveProxy(ProxyTable.ActorFacadeProxy) 
end      
--�������Ľ���ǣ�������ƶ����ͻ���ÿ�������ҷ���һ���ƶ�״̬ˢ�¡�
function PlayerManagerProxy:Step(time) --���ý�ɫ���ƶ�λ��
    local playerData = sol.PlayerMan.Instance():PoPWaitPlayer()
    if not playerData then 
        return 
    end  
    --��ʼ���������ɫ 
    local actor = self.ActorFacadeProxy:CreateActor(2,2,sol.b2Vec2.new(0,0),1) --����һ������IDΪ2�����     
    self.NavmeshManagerProxy:RegisterActor(actor)  
    playerData:SetBodyData(actor:GetBody())  
    sol.PlayerMan.Instance():PlayerEnterGame(playerData) 
end
 
return PlayerManagerProxy 