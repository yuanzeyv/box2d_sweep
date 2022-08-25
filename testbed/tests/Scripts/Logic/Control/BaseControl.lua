local BaseControl = class("BaseControl",framework.SimpleCommand) 

function BaseControl:execute(notification)
    self:InitData()
    self:Execute(notification)
end

--用于数据的初始化
function BaseControl:InitData() 

end  
function BaseControl:Execute(notification)
    
end
return BaseControl  