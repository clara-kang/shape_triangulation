#include "Shape.hpp"

Shape::Shape(BezierCurve *curve) {
	curves.push_back(curve);
};

BezierPoint *Shape::getHead() {
	return &(curves[0]->getStart());
}

BezierPoint *Shape::getTail() {
	return &(curves.end()[-1]->getEnd());
}

// check whether the given curve belongs to shape
bool Shape::curveInShape(BezierCurve *curve) {
	for (std::vector<BezierCurve*>::iterator it = curves.begin(); it != curves.end(); ++it) {
		if ((*it) == curve) {
			return true;
		}
	}
	return false;
}

// put all curves of the other shape to the front
void Shape::mergeToHead(Shape *to_merge) {
	// if merging two different curves
	if (this != to_merge) {
		curves.insert(curves.begin(), to_merge->curves.begin(), to_merge->curves.end());
	}
	// if merging with oneself
	else {
		completed = true;
	}
}

// put all curves of the other shape to the end
void Shape::mergeToTail(Shape *to_merge) {
	// if merging two different curves
	if (this != to_merge) {
		curves.insert(curves.end(), to_merge->curves.begin(), to_merge->curves.end());
	}
	// if merging with oneself
	else {
		completed = true;
	}
}

// get nb
BezierPoint *Shape::getNbPoint(BezierPoint *bp) {
	BezierPoint *nb = NULL;
	if (bp == getHead() && this->completed) {
		return getTail();
	}
	else if (bp == getTail() && this->completed) {
		return getHead();
	}
	else if ((bp == getHead() || bp == getTail()) && !this->completed) {
		return NULL;
	}
	else {
		for (auto it = curves.begin(); it != curves.end(); ++it) {
			if (&(*it)->getEnd() == bp && it != curves.end() - 1) {
				nb = &((*(it + 1))->getStart());
				break;
			}
		}
		return nb;
	}
}

bool Shape::isClockWise() {
	if (curves.size() < 2) {
		throw std::exception("less than 2 curves, cannot determine orientation");
		return false;
	}
	return false;
}