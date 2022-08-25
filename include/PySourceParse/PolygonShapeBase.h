#ifndef _POLYGON_SHAPE_H_
#define _POLYGON_SHAPE_H_ 
#include "ShapeBase.h"   
class PolygonShapeBase :public ShapeBase {
public: 
	vector<b2Vec2> PointVec;
	PolygonShapeBase() :ShapeBase() {}
	virtual void InitShape()
	{
		b2PolygonShape *PolygonShape = new b2PolygonShape(); 
		PolygonShape->Set((b2Vec2*)PointVec.data(),(int32) PointVec.size()); 
		Shape = PolygonShape;
	}
}; 
#endif //  _POLYGON_SHAPE_H_
