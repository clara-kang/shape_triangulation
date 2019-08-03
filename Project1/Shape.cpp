#pragma once
#include "Shape.hpp"
#include "MathUtil.hpp"

Shape::Shape(BezierCurve *curve) {
	curves.push_back(curve);
};

const BezierPoint *Shape::getHead() {
	return &(curves[0]->getStart());
}

const BezierPoint *Shape::getTail() {
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

//// get nb, for example curve1 end and curve2 start are nbs
//BezierPoint *Shape::getNbPoint(BezierPoint *bp) {
//	BezierPoint *nb = NULL;
//	if (bp == getHead() && this->completed) {
//		return getTail();
//	}
//	else if (bp == getTail() && this->completed) {
//		return getHead();
//	}
//	else if ((bp == getHead() || bp == getTail()) && !this->completed) {
//		return NULL;
//	}
//	else {
//		for (auto it = curves.begin(); it != curves.end(); ++it) {
//			if (&(*it)->getEnd() == bp && it != curves.end() - 1) {
//				nb = &((*(it + 1))->getStart());
//				break;
//			}
//		}
//		return nb;
//	}
//}

BezierCurve *Shape::getNextCurve(BezierCurve *c) {
	if (c == curves.end()[-1] && this->completed) {
		return curves[0];
	}
	else if (c == curves.end()[-1] && !this->completed) {
		return NULL;
	}
	else {
		for (int i = 0; i < curves.size() - 1; i++) {
			if (curves[i] == c) {
				return curves[i + 1];
			}
		}
		throw std::exception("cannot find curve");
	}
}

BezierCurve *Shape::getPrevCurve(BezierCurve *c) {
	if (c == curves[0] && this->completed) {
		return curves.end()[-1];
	}
	else if (c == curves[0] && !this->completed) {
		return NULL;
	}
	else {
		for (int i = 1; i < curves.size(); i++) {
			if (curves[i] == c) {
				return curves[i - 1];
			}
		}
		throw std::exception("cannot find curve");
	}
}

glm::vec2 getV1(BezierCurve *curve) {
	if (curve->getType() == BezierCurve::type::CUBIC) {
		return curve->getStart().ctrl_loc - curve->getStart().loc;
	}
	else {
		return curve->getEnd().loc - curve->getStart().loc;
	}
}

bool Shape::isClockWise() {
	float winding_num = 0.f;
	for (int i = 0; i < curves.size(); i++) {
		BezierCurve *curve = curves[i];
		glm::vec2 lastV;
		// cubic curves approx by ctrl poitns
		if (curve->getType() == BezierCurve::type::CUBIC) {
			glm::vec2 v1 = curve->getStart().ctrl_loc - curve->getStart().loc;
			glm::vec2 v2 = curve->getEnd().ctrl_loc - curve->getStart().ctrl_loc;
			glm::vec2 v3 = curve->getEnd().loc - curve->getEnd().ctrl_loc;
			// accumulate winding number
			winding_num += getSignedAngle(v1, v2);
			winding_num += getSignedAngle(v2, v3);
			lastV = v3;
		}
		else {
			// linear curve
			lastV = curve->getEnd().loc - curve->getStart().loc;
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