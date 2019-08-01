#pragma once
#include "PointUtil.hpp"
#include "MathUtil.hpp"
#include <algorithm>
#include <iostream>
#include <list>

PointUtil::PointUtil(Shape *shape, float Lm) {
	this->shape = shape;
	this->Lm = Lm;
	this->Lb = Lm * sqrt(2.f);
}

void PointUtil::computePm() {
	bool clockWise = shape->isClockWise();
	for (auto it = (shape->curves).begin(); it != (shape->curves).end(); ++it) {
		BezierCurve *curve = *it;
		glm::vec2 loc = curve->getEnd().loc;
		glm::vec2 normal;
		// set vertex points
		Point *Pv = new Point(loc, Point::Type::VERTEX);
		Pm.push_back(Pv);

		// set edge points
		float curveLen = curve->getLength();
		// how segments needed
		int segNum = curveLen / Lb;
		// how long segment between two E points is
		float segLen = curveLen / (float)segNum;
		for (int i = 1; i < segNum; i++) {
			float t;
			loc = curve->getPointAtLength(segLen * (float)i, t);
			normal = curve->getNormalAtT(t, clockWise);
			Point *Pe = new Point(loc, normal, Point::Type::EDGE);
			// copy point to Pm
			Pm.push_back(Pe);
			// copy point to Pw to process later
			Pw.push_back(Pe);
		}
	}
	mergePbs();
	// set internal points
	computeIPoints();
}

void PointUtil::rmPfromPm(Point *P) {
	auto it = std::find(Pm.begin(), Pm.end(), P);
	auto it_w = std::find(Pw.begin(), Pw.end(), P);
	if (it_w != Pw.end()) {
		Pw.erase(it_w);
	}
	if (it != Pm.end()) {
		Pm.erase(it);
	}
	else {
		throw std::exception("cannot find P to delete from PM");
	}
}

void nullFromVector(std::vector<Point *> &v, Point *P) {
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == P) {
			v[i] = NULL;
			return;
		}
	}
}

void clearNulls(std::vector<Point *> &v) {
	for (auto it = v.begin(); it != v.end();) {
		if (*it == NULL) {
			it = v.erase(it);
		}
		else {
			++it;
		}
	}
}

void PointUtil::mergePbs() {
	for (int i = 0; i < Pm.size(); i++) {
		if (Pm[i] == NULL) {
			continue;
		}
		Point *p1 = Pm[i];
		for (int j = 0; j < Pm.size(); j++) {
			if (i == j || Pm[j] == NULL) {
				continue;
			}
			Point *p2 = Pm[j];
			// not itself
			float dist = glm::distance(p1->loc, p2->loc);
			if (dist <= Lm) {
				// if p1 v-point, p2 v-point or e-point, remove p2
				if (p1->type == Point::Type::VERTEX) {
					nullFromVector(Pm, p2);
					nullFromVector(Pw, p2);
					delete p2;
					continue;
				}
				// if p2 v-point, p1 v-point or e-point, remove p1
				else if (p1->type == Point::Type::EDGE) {
					nullFromVector(Pm, p1);
					nullFromVector(Pw, p2);
					delete p1;
					break;
				}
			}
		}
	}
	clearNulls(Pm);
	clearNulls(Pw);
}

void PointUtil::getPf(Point *P, Point *Pf) {
	Pf->loc = P->loc + P->normal * Lb;
	Pf->normal = P->normal;
	Pf->type = Point::Type::INTERNAL;
}

void PointUtil::getPl(Point *P, Point *Pl) {
	glm::vec2 left_dir = rotate90cw(P->normal);
	Pl->loc = P->loc + left_dir * Lb;
	Pl->normal = left_dir;
	Pl->type = Point::Type::INTERNAL;
}

void PointUtil::getPr(Point *P, Point *Pr) {
	glm::vec2 right_dir = rotate90ccw(P->normal);
	Pr->loc = P->loc + right_dir * Lb;
	Pr->normal = right_dir;
	Pr->type = Point::Type::INTERNAL;
}

void PointUtil::getPflr(Point *P, Point *Pf, Point *Pl, Point *Pr) {
	getPf(P, Pf);
	getPl(P, Pl);
	getPr(P, Pr);
}

bool PointUtil::getClosestInPm(Point *P, Point **closeP) {
	auto dist2P = [P](const Point *p1, const Point *p2) {
		// set the distance from itself to something large
		float dist2p1, dist2p2;
		if (p1 == P) {
			dist2p1 = INFINITY;
		}
		else {
			dist2p1 = glm::distance(P->loc, p1->loc);
		}
		if (p2 == P) {
			dist2p2 = INFINITY;
		}
		else {
			dist2p2 = glm::distance(P->loc, p2->loc);
		}
		return dist2p1 < dist2p2;
		
	};
	auto res_it = std::min_element(Pm.begin(), Pm.end(), dist2P);
	float min_dist = glm::distance(P->loc, (*res_it)->loc);
	*closeP = (*res_it);
	if (min_dist >= Lm) {
		return false;
	}
	if (res_it == Pm.end()) {
		throw std::exception("no min found?");
		return false;
	}
	return true;
}

glm::vec2 takeNormalAvg(glm::vec2 v1, glm::vec2 v2) {
	float angle = getSignedAngle(v1, v2);
	if (v1 == v2 || angle ==0.f || isnan(angle)) {
		//throw std::exception("angle is nan");
		std::cout << "angle is nan" << std::endl;
		return glm::normalize(v1);
	}
	return rotate(v1, angle / 2.f);
}

void takeAvg(Point *P1, Point *P2, Point *Pres) {
	Pres->loc = 0.5f * (P1->loc + P2->loc);
	Pres->normal = takeNormalAvg(P1->normal, P2->normal);
	Pres->type = Point::Type::INTERNAL;
}

// P1 has not been added to Pm yet
bool PointUtil::mergeTwoPoints(Point *P1, Point *P2, Point *Pres) {
	if (P1->type == Point::Type::INTERNAL) {
		if (P2->type == Point::Type::VERTEX || P2->type == Point::Type::EDGE) {
			// remove P1
			return false;
		}
		else if (P2->type == Point::Type::INTERNAL) {
			// take avg of P1, P2
			takeAvg(P1, P2, Pres);
			// remove P1, P2
			rmPfromPm(P2);
			delete P2;
			return true;
		}
	}
	else if (P2->type == Point::Type::INTERNAL) {
		// P1 must be on boundary, remove P2
		rmPfromPm(P2);
		delete P2;
		return false;
	}
	else {
		throw std::exception("both P1 P2 are on boundary, should not happen for internal points merging");
		return false;
	}
}

// return true if a point survives
bool PointUtil::mergeWPm(Point *P, Point *Pres) {
	Point *closeP;
	bool needMerge = getClosestInPm(P, &closeP);
	// no point within Lm distance
	if (!needMerge) {
		// copy content
		std::swap(*Pres, *P);
		Pm.push_back(Pres);
		return true;
	}
	// closeP is within Lm distance
	else if (needMerge && mergeTwoPoints(P, closeP, Pres)){
		while (true) {
			needMerge = getClosestInPm(Pres, &closeP);
			if ( needMerge && mergeTwoPoints(Pres, closeP, Pres)) {
				continue;
			}
			else {
				break;
			}
		}
		Pm.push_back(Pres);
		return true;
	}
	return false;
}

void PointUtil::computeIPoints() {
	while (Pw.size() > 0) {
		Point *point = Pw[0];
		Point Pf, Pl, Pr;
		getPflr(point, &Pf, &Pl, &Pr);
		std::vector<Point> Pnbs({ Pf, Pl, Pr });
		for (int i = 0; i < 3; i++) {
			Point *Pres = new Point();
			// check Pf, Pl, Pr outside of shape
			// if merging with Pm results in Pres
			if ( shape->pInShape(Pnbs[i].loc) && mergeWPm(&Pnbs[i], Pres)) {
				Pm.push_back(Pres);
				Pw.push_back(Pres);
			}
			else {
				delete Pres;
			}
		}
		Pw.erase(Pw.begin());
		// delete later
		window->clear();
		render(*window);
		window->display();
	}
}

void PointUtil::passWindow(sf::RenderWindow *window) {
	this->window = window;
}

void PointUtil::render(sf::RenderWindow &window) {
	for (auto it = Pm.begin(); it != Pm.end(); ++it) {
		//if ((*it)->type == Point::Type::INTERNAL) {
		//	(*it)->render(window);
		//}
		(*it)->render(window);
	}
}