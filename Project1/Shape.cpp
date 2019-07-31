#pragma once
#include "Shape.hpp"
#include "MathUtil.hpp"

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

glm::vec2 getV1(BezierCurve *curve) {
	if (curve->getType() == BezierCurve::type::CUBIC) {
		return curve->start.ctrl_loc - curve->start.loc;
	}
	else {
		return curve->end.loc - curve->start.loc;
	}
}

bool Shape::isClockWise() {
	float winding_num = 0.f;
	for (int i = 0; i < curves.size(); i++) {
		BezierCurve *curve = curves[i];
		glm::vec2 lastV;
		// cubic curves approx by ctrl poitns
		if (curve->getType() == BezierCurve::type::CUBIC) {
			glm::vec2 v1 = curve->start.ctrl_loc - curve->start.loc;
			glm::vec2 v2 = curve->end.ctrl_loc - curve->start.ctrl_loc;
			glm::vec2 v3 = curve->end.loc - curve->end.ctrl_loc;
			// accumulate winding number
			winding_num += getSignedAngle(v1, v2);
			winding_num += getSignedAngle(v2, v3);
			lastV = v3;
		}
		else {
			// linear curve
			lastV = curve->end.loc - curve->start.loc;
		}
		glm::vec2 nextV = getV1(curves[(i + 1) % curves.size()]);
		winding_num += getSignedAngle(lastV, nextV);
	}
	if (winding_num > 0) {
		return true;
	}
	return false;
}

bool Shape::pInShape(glm::vec2 &ploc) {
	// should a random direction be used?
	glm::vec2 ray_dir(1.0, 0.0);
	int intrsct_times = 0;
	for (auto it = curves.begin(); it != curves.end(); ++it) {
		if ((*it)->intersect(ploc, ray_dir)) {
			intrsct_times++;
		}
	}
	if (intrsct_times % 2 == 1) {
		return true;
	}
	return false;
}