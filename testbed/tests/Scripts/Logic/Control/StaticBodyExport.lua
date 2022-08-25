local BaseControl = require("Logic.Control.BaseControl")
local StaticBodyExport = class("StaticBodyExport",BaseControl)  
function StaticBodyExport:GetRealShape(fixture)
    local TypeFuncMap = {[EnumDefine.b2ShapeType.Circle] ="GetRealCircle" ,[EnumDefine.b2ShapeType.Polygon] = "GetRealPolygon"} 
    local shapeType = fixture:GetType() 
    if not TypeFuncMap[shapeType] then return false end 
    local shape = fixture:GetShape()--循环获取到当前刚体中的所有的形状
    shape = shape[TypeFuncMap[shapeType]](shape) 
    return shape--获取真是形状 
end 
function StaticBodyExport:GetStaticShapes()--获取到当前静态地图上的所有顶点信息
    local i = 1 
    local shapesList = {} --顶点数组,每个形状对于一个顶点数组
    local staticBodyTable = sol.BodyMan.Instance():GetBodyList(EnumDefine.BodyType.StaticBody)--获取到当前地图上的所有今天物体
    for v,bodyData in pairs(staticBodyTable) do 
        local body = bodyData:GetBody() --首先获取到的昂前的缸体 
        local bodyPos = body:GetPosition()--静态物体是相对于刚体坐标的
        local bodyRotate = body:GetAngle()--获取角度
        local rotateRad = math.rad(bodyRotate)--获取弧度
        local fixture = body:GetFixtureList()--获取所有夹具信息
        while(fixture) do--循环遍历夹具 
            local shape = self:GetRealShape(fixture)
            local points = nil
            if not shape then  
                goto Next
            end 
            points = shape:GetVectors(30) --圆形才需要这个参数,数值越大,形状越圆  
            i=i+ 1
            for index,vec in pairs(points) do    
                vec.x = vec.x + bodyPos.x
                vec.y = vec.y + bodyPos.y  
                vec.x= (vec.x - bodyPos.x)*math.cos(rotateRad)- (vec.y - bodyPos.y)*math.sin(rotateRad) + bodyPos.x ;
                vec.y= (vec.x - bodyPos.x)*math.sin(rotateRad)+ (vec.y - bodyPos.y)*math.cos(rotateRad) + bodyPos.y ;
                vec.x = vec.x 
                vec.y = -vec.y
            end 
            table.insert(shapesList,points)
            ::Next:: 
            fixture = fixture:GetNext() 
        end 
    end   
    return shapesList
end 

function StaticBodyExport:GetAABBBox(shapes)--获取到当前静态地图上的所有顶点信息
    local leftLowerPos = sol.b2Vec2.new(999999,999999)
    local rightTopPos = sol.b2Vec2.new(-999999,-999999) 
    local rightLowerPos = sol.b2Vec2.new(0,0)
    local leftTopPos = sol.b2Vec2.new(0,0)
    for v,k in pairs(shapes) do --获取到网格 
        for _,point in pairs(k) do --读取添加到每个点
            local xPos = point.x
            local yPos = point.y
            if xPos < leftLowerPos.x then 
                leftLowerPos.x = xPos - 1
                leftTopPos.x = leftLowerPos.x 
            end 
            if yPos < leftLowerPos.y then 
                leftLowerPos.y = yPos - 1
                rightLowerPos.y = leftLowerPos.y
            end 
            if  xPos > rightTopPos.x then 
                rightTopPos.x = xPos + 1 
                rightLowerPos.x = rightTopPos.x
            end 
            if  yPos > rightTopPos.y then 
                rightTopPos.y = yPos + 1
                leftTopPos.y = rightTopPos.y
            end   
        end
    end
    return {leftLowerPos,leftTopPos,rightTopPos,rightLowerPos}
end
--输出顶点文件
function StaticBodyExport:OutPutDelabellaFile(fileName,statisShapes,groundPoint)
    local indexTable = {} --索引顶点
    local valueTable = {} --内容表,用于顶点复用
    local edgeTable = {} --边表
    local index = 0 --下标由零开始
    for v,k in pairs(statisShapes) do --循环遍历所有的形状
        for _,point in pairs(k) do --读取添加到每个点 
            if not valueTable[point.x .. ":" .. point.y] then --如果当前没有指定的index的话
                valueTable[point.x .. ":" .. point.y]  = index --为当前点位赋值 
                indexTable[index + 1] = point 
                index = index + 1 
            end
        end
        for i = 1,#k,1 do
            local point = k[i] 
            local nextPoint = k[ (i % #k) + 1 ] 
            edgeTable[#edgeTable + 1 ] = {valueTable[point.x .. ":" .. point.y],valueTable[nextPoint.x .. ":" .. nextPoint.y]}
        end  
    end
    for v,point in pairs(groundPoint)  do 
        if not valueTable[point.x .. ":" .. point.y] then --如果当前没有指定的index的话
            valueTable[point.x .. ":" .. point.y]  = index --为当前点位赋值 
            indexTable[index + 1] = point 
            index = index + 1 
        end
    end 
    file=io.open(fileName,"w")
    file:write((#indexTable ) .. " " .. #edgeTable.."\n") 
    for i = 1,#indexTable  do 
        file:write(indexTable[i].x .. " " ..indexTable[i].y.."\n")
    end   
    for v,k in pairs(edgeTable) do 
        file:write(k[1].. " " .. k[2].."\n")
    end 
    file:close()
end 
--用于数据的初始化
function StaticBodyExport:InitData()   
    local statisShapes = self:GetStaticShapes()--获取到所有静态形状
    local groundPoint = self:GetAABBBox(statisShapes)--获取到边缘顶点  
    self:OutPutDelabellaFile("./a.txt",statisShapes,groundPoint)--输出顶点文件到表
    sol.Delabella.Generate3DMap("./a",false)--完成后,调用接口,输出反向的图形
end  

function StaticBodyExport:Execute(notification)   
end 
return StaticBodyExport