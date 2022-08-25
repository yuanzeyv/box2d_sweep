local InitWorldControl = class("InitWorldProxy") 
function InitWorldControl:ctor()
    self.ControlPathTable = {} 
    self.ControlTable = {} 
    self.BasePath = "Logic.Control." 

    self:InitWorldControlTable()
end 
function InitWorldControl:InitWorldControlTable() 
    self:RegisterControl(NotifyDefine.StartGameControl,"StartGameControl")
    self:RegisterControl(NotifyDefine.StaticBodyExportControl,"StaticBodyExport") 
    self:RegisterControl(NotifyDefine.ResourceLoad,"ResourceLoad") 
end 

function InitWorldControl:RegisterControl(name,path)  
    self.ControlTable[name] = self.BasePath .. path 
end 
function InitWorldControl:InitControl() 
    for name,path in pairs(self.ControlTable) do 
        G_FACADE:registerCommand(name, require(path))
    end   
end 
return InitWorldControl  