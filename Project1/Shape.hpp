#pragma once
#include <vector>
#include "BezierCurve.hpp"

// todo: cannot form shape with < 3 lines

class Shape {
public:
	Shape() {};
	Shape(BezierCurve *curve);

	// whether shape form a loop
	bool completed;
	// whether clock-wise or counter-clock-wise
	bool cw;
	// the curves that the shape contains
	std::vector<BezierCurve *> curves;
	
	const BezierPoint *getHead();

	const BezierPoint *getTail();

	// check whether the given curve belongs to shape
	bool curveInShape(BezierCurve *curve);

	// put all curves of the other shape to the front
	void mergeToHead(Shape *to_merge);

	// put all curves of the other shape to the end
	void mergeToTail(Shape *to_merge);

	// positively oriented
	bool isClockWise();

	// point within shape
	bool pInShape(glm::vec2 &ploc);

	// get nb
	//BezierPoint *getNbPoint(BezierPoint *bp);

	// get next curve
	BezierCurve *getNextCurve(BezierCurve *c);
	// get previous curve
	BezierCurve *getPrevCurve(BezierCurve *c);
};