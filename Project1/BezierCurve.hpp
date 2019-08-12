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
	BezierCurve() {};
	BezierCurve(BezierPoint start, BezierPoint end);

	const BezierPoint &getStart();
	const BezierPoint &getEnd();
	
	virtual void moveStartPos(glm::vec2 pos);
	virtual void moveEndPos(glm::vec2 pos);
	virtual void moveStartCtrl(glm::vec2 pos);
	virtual void moveEndCtrl(glm::vec2 pos);

	virtual float getLength() = 0;
	virtual glm::vec2 getPointAtLength(float length) = 0;
	virtual glm::vec2 getPointAtLength(float length, float &stopt) = 0;
	virtual glm::vec2 getNormalAtT(float t, bool cw) = 0;
	//virtual bool intersect(glm::vec2 ray_start, glm::vec2 ray_dir, glm::vec2 &intrsctn) = 0;
	virtual bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir) = 0;
	virtual bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir, float &dist) = 0;
	virtual void render(sf::RenderWindow &window) = 0;
	virtual void renderSelected(sf::RenderWindow &window) = 0;
	virtual type getType() = 0;
protected:
	BezierPoint start;
	BezierPoint end;
	float length = 0;
	void renderPts(sf::RenderWindow &window);
};

class Linear : public BezierCurve {
public:
	Linear() {};
	Linear(BezierPoint start, BezierPoint end);
	BezierCurve::type getType();

	void render(sf::RenderWindow &window);
	void renderSelected(sf::RenderWindow &window);
	float getLength();
	glm::vec2 getPointAtLength(float length);
	glm::vec2 getPointAtLength(float length, float &stopt);
	glm::vec2 getNormalAtT(float t, bool cw);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir, glm::vec2 &intrsctn, float &dist);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir, float &dist);
private:
	bool hasNormal = false;
	glm::vec2 normal = glm::vec2(0.f);
};

class Cubic : public BezierCurve {
public:
	Cubic(BezierPoint start, BezierPoint end);
	BezierCurve::type getType();
	void moveStartPos(glm::vec2 pos) override;
	void moveEndPos(glm::vec2 pos) override;
	void moveStartCtrl(glm::vec2 pos) override;
	void moveEndCtrl(glm::vec2 pos) override;

	void render(sf::RenderWindow &window);
	void renderSelected(sf::RenderWindow &window);
	float getLength();
	glm::vec2 getPointAtLength(float length);
	glm::vec2 getPointAtLength(float length, float &stopt);
	glm::vec2 getNormalAtT(float t, bool cw);
	//bool intersect(glm::vec2 ray_start, glm::vec2 ray_dir, glm::vec2 &intrsctn);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir);
	bool intersect(glm::vec2 &ray_start, glm::vec2 &ray_dir, float &dist);
private:
	// start.loc: 0, start.ctrl_loc: 1, end.ctrl_loc: 2, end.loc: 3, the order of them in the convex hull
	std::vector<int> chIndices;
	glm::vec2 getPointAtT(float t);
	float getLengthAtT(float tstop);
	// compute convex hull of ctrl points
	void getConvexHull();
};