#ifndef _CIRCLE_SHAPE_H_
#define _CIRCLE_SHAPE_H_ 
#include "ShapeBase.h"   
class CircleShapeBase :public ShapeBase {
public:
	float Radius;
	float X;
	float Y;
	CircleShapeBase():ShapeBase(){
		Radius = 1;
		X = 0;
		Y = 0;
	}
	virtual void InitShape()
	{ 
		b2CircleShape* CircleShape = new b2CircleShape();
		CircleShape->m_radius = Radius;
		CircleShape->m_p = b2Vec2(X, Y );
		Shape = CircleShape;
	} 
};  
#endif //_CIRCLE_SHAPE_H_