#pragma once
#include <glm/glm.hpp>
#include <math.h> 
#include <vector>
#include "BezierPoint.hpp"
#include <string>

static int STEPS = 50;

class BezierCurve {
public:
	enum class type {
		LINEAR,
		CUBIC
	};
	BezierPoint start;
	BezierPoint end;
	BezierCurve() {};
	BezierCurve(BezierPoint start, BezierPoint end);

	BezierPoint &getStart();
	BezierPoint &getEnd();
	
	virtual float getLength() = 0;
	virtual glm::vec2 getPointAtLength(float length) = 0;
	virtual glm::vec2 getPointAtLength(float length, float &stopt) = 0;
	virtual glm::vec2 getNormalAtT(float t, bool cw) = 0;
	//virtual bool intersect(glm::vec2 ray_start, glm::vec2 ray_dir, glm::vec2 &intrsctn) = 0;
	virtual bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir) = 0;
	virtual void render(sf::RenderWindow &window) = 0;
	virtual type getType() = 0;
protected:
	float length = 0;
	void renderPts(sf::RenderWindow &window);
};

class Linear : public BezierCurve {
public:
	Linear() {};
	Linear(BezierPoint start, BezierPoint end);
	BezierCurve::type getType();
	void render(sf::RenderWindow &window);
	float getLength();
	glm::vec2 getPointAtLength(float length);
	glm::vec2 getPointAtLength(float length, float &stopt);
	glm::vec2 getNormalAtT(float t, bool cw);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir, glm::vec2 &intrsctn);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir);
private:
	glm::vec2 normal;
};

class Cubic : public BezierCurve {
public:
	Cubic(BezierPoint start, BezierPoint end);
	BezierCurve::type getType();
	void render(sf::RenderWindow &window);
	float getLength();
	glm::vec2 getPointAtLength(float length);
	glm::vec2 getPointAtLength(float length, float &stopt);
	glm::vec2 getNormalAtT(float t, bool cw);
	//bool intersect(glm::vec2 ray_start, glm::vec2 ray_dir, glm::vec2 &intrsctn);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir);
private:
	glm::vec2 getPointAtT(float t);
	float getLengthAtT(float tstop);
};