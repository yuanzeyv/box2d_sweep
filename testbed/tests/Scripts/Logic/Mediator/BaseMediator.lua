local BaseMediator = class("BaseMediator",framework.Mediator)     
function BaseMediator:ctor()
    framework.Mediator.ctor(self)
    self._ListenNotificationList = {}  
    self:NotifycationRegister(self._ListenNotificationList) 
end 

function BaseMediator:LoadFinish()  
    self:InitData() 
end

function BaseMediator:onRemove()
    self:ClearData()
end


function BaseMediator:listNotificationInterests()
    local notificationList = {}        
    for k,v in pairs(self._ListenNotificationList) do 
       table.insert(notificationList,k)
    end  
    return notificationList
end

function BaseMediator:handleNotification(notification) 
    local handle = self._ListenNotificationList[notification:getName()]     
    if handle then  
        handle(notification:getBody())
    end     
end 

--被清理是
function BaseMediator:ClearData()  end  
function BaseMediator:NotifycationRegister(table) 
end 
--用于数据的初始化
function BaseMediator:InitData() 
end 
 

return BaseMediator
