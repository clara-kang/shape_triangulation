#pragma once
#include <glm/glm.hpp>
#include <math.h> 
#include <vector>
#include "BezierPoint.hpp"

static int STEPS = 50;

class BezierCurve {
public:
	BezierPoint start;
	BezierPoint end;
	BezierCurve(BezierPoint start, BezierPoint end);

	BezierPoint &getStart();
	BezierPoint &getEnd();
	
	virtual float getLength() = 0;
	virtual glm::vec2 getPointAtLength(float length) = 0;
	virtual glm::vec2 getNormalAtT(float t, bool cw) = 0;
	virtual void render(sf::RenderWindow &window) = 0;

protected:
	float length = 0;
	void renderPts(sf::RenderWindow &window);
};

class Linear : public BezierCurve {
public:
	Linear(BezierPoint start, BezierPoint end);
	void render(sf::RenderWindow &window);
	float getLength();
	glm::vec2 getPointAtLength(float length);
	glm::vec2 getNormalAtT(float t, bool cw);
private:
	glm::vec2 normal;
};

class Cubic : public BezierCurve {
public:
	Cubic(BezierPoint start, BezierPoint end);
	void render(sf::RenderWindow &window);
	float getLength();
	glm::vec2 getPointAtLength(float length);
	glm::vec2 getNormalAtT(float t, bool cw);

private:
	glm::vec2 getPointAtT(float t);
	float getLengthAtT(float tstop);
};