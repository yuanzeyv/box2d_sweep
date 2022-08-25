#ifndef _SHAPE_BASE_H_
#define _SHAPE_BASE_H_
#include "../box2d/box2d.h"
class ShapeBase {
public:
	b2Shape* Shape;
	ShapeBase() { 
		Shape = NULL;
	}
	virtual void InitShape() = 0; 
	virtual ~ShapeBase()
	{
		if (Shape)
			delete Shape;
	}
}; 
#endif // 
