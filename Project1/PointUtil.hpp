#pragma once
#include "Shape.hpp"
#include <vector>
#include "Point.hpp"

class PointUtil {
public:
	Shape *shape; // boundary shape
	std::vector<Point> Pm; // set of points
	std::vector<Point> Pw; // points that still need to be propagated
	float Lm; // distance pair of points are from each other

	PointUtil(Shape *shape, float Lm);
	void computePm();
	void render(sf::RenderWindow &window);
};
