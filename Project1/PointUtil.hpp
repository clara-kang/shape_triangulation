#pragma once
#include "Shape.hpp"
#include <vector>
#include "Point.hpp"

class PointUtil {
public:
	Shape *shape; // boundary shape
	std::vector<Point *> Pm; // set of points
	std::vector<Point *> Pw; // points that still need to be propagated
	float Lm; // min distance pair of points are from each other
	float Lb; // default point distance
	PointUtil(Shape *shape, float Lm);
	void computePm();
	void render(sf::RenderWindow &window);
	//delete later
	void passWindow(sf::RenderWindow *window);
	std::vector<glm::vec2> points2pos();
private:
	void computeIPoints();
	void mergePbs();
	void getPf(Point *P, Point *Pf);
	void getPl(Point *P, Point *Pl);
	void getPr(Point *P, Point *Pr);
	void getPflr(Point *P, Point *Pf, Point *Pl, Point *Pr);
	bool getClosestInPm(Point *P, Point **closeP);
	bool mergeTwoPoints(Point *P1, Point *P2, Point *Pres);
	bool mergeWPm(Point *P, Point *Pres);
	void rmPfromPmPw(Point *P);
	// delete later
	sf::RenderWindow *window;
};
