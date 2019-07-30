#include "PointUtil.hpp"

PointUtil::PointUtil(Shape *shape, float Lm) {
	this->shape = shape;
	this->Lm = Lm;
}

void PointUtil::computePm() {
	bool clockWise = shape->isClockWise();
	for (auto it = (shape->curves).begin(); it != (shape->curves).end(); ++it) {
		BezierCurve *curve = *it;
		glm::vec2 loc = curve->getEnd().loc;
		glm::vec2 normal;
		// set vertex points
		Pm.push_back(Point(loc, Point::Type::VERTEX));

		// set edge points
		float curveLen = curve->getLength();
		// how segments needed
		int segNum = curveLen / Lm;
		// how long segment between two E points is
		float segLen = curveLen / (float)segNum;
		for (int i = 1; i < segNum; i++) {
			float t;
			loc = curve->getPointAtLength(segLen * (float)i, t);
			normal = curve->getNormalAtT(t, clockWise);
			Pm.push_back(Point(loc, normal, Point::Type::EDGE));
		}

		// set internal points
	}

}

void PointUtil::render(sf::RenderWindow &window) {
	for (auto it = Pm.begin(); it != Pm.end(); ++it) {
		if ((*it).type == Point::Type::EDGE) {
			(*it).render(window);
		}
		//(*it).render(window);
	}
}