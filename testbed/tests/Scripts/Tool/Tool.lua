function Split(str,reps)
    local resultStrList = {}
    string.gsub(str,'[^'..reps..']+',function (w)
        table.insert(resultStrList,w)
    end)
    return resultStrList
end

--一个回调函数
function handler(obj, method)
    return function(...) 
        return method(obj, ...)
    end
end 

function SplitParam(str,flag )  
    local attrCellTable = Split(str,flag or "|")--首先分离属性  
    local table = {} 
    for i = 1 , #attrCellTable do 
        table[ "param" .. i ] =  attrCellTable[i] 
    end   
    return table
end 

function SplitB2Vec2(posStr)
    local pos = Split(posStr,"|")  
    return sol.b2Vec2.new(tonumber( pos[1]),tonumber( pos[2]))  
end
 

function SplitAttribute(attr) 
    if not attr then  return nil end 
    local triggerTable = {}
    local attrTable = Split(attr,"#")--首先分离属性 
    for v,k in pairs(attrTable) do
        local attrCellTable = Split(k,"|")--首先分离属性 
        local key = tonumber(attrCellTable[1])
        triggerTable[key] = {}  
        for i = 2 , #attrCellTable do 
            triggerTable[key][ "param" .. (i-1) ] =  attrCellTable[i] 
        end
    end
    return triggerTable
end 

function GetTableKey(inputTable)
    local keyTable = {} 
    for v,k in pairs(inputTable) do 
        table.insert( keyTable, v) 
    end 
    return keyTable
end  

function InsertLine(now,final,percent)
    return (final - now) * percent
end 


function ThreadRunning(conditionFunc)
    while(conditionFunc()) do --为假的时候代表工作结束了
        coroutine.yield( EnumDefine.NodeStatus.RUNNING )
    end 
end  