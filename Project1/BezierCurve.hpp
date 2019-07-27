#pragma once
#include <glm/glm.hpp>
#include <math.h> 
#include <vector>
#include "BezierPoint.hpp"

class BezierCurve {
public:
	BezierPoint start;
	BezierPoint end;
	BezierCurve(BezierPoint start, BezierPoint end);
	glm::vec2 getStart();
	glm::vec2 getEnd();
	virtual void render(sf::RenderWindow &window) = 0;
protected:
	void renderPts(sf::RenderWindow &window);
};

class Linear : public BezierCurve {
public:
	Linear(BezierPoint start, BezierPoint end);
	void render(sf::RenderWindow &window);
};

class Cubic : public BezierCurve {
public:
	Cubic(BezierPoint start, BezierPoint end);
	void render(sf::RenderWindow &window);
};