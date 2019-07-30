#pragma once
#include "BezierPoint.hpp"
#include "BezierCurve.hpp"

float POINT_RADIUS = 5.f;
float ERROR_FRAC = 0.01;

BezierCurve::BezierCurve(BezierPoint start, BezierPoint end) {
	this->start = start;
	this->end = end;
}

BezierPoint &BezierCurve::getStart() {
	return start;
};

BezierPoint &BezierCurve::getEnd() {
	return end;
}

void BezierCurve::renderPts(sf::RenderWindow &window) {
	start.render(window);
	end.render(window);
}

Linear::Linear(BezierPoint start, BezierPoint end) : BezierCurve(start, end) {}

float Linear::getLength() {
	if (length == 0) {
		length = glm::distance(start.loc, end.loc);
	}
	return length;
}

glm::vec2 Linear::getPointAtLength(float length) {
	return start.loc + glm::normalize(end.loc - start.loc) * length;
}

glm::vec2 Linear::getNormalAtT(float t, bool cw) {
	// normal not defined
	if (glm::length(normal) < 1.f) {
		glm::vec2 s2e = end.loc - start.loc;
		glm::vec2 normal_nn;
		if (cw) {
			normal_nn = glm::vec2(s2e.y, -s2e.x);
		}
		else {
			normal_nn = glm::vec2(-s2e.y, s2e.x);
		}
		normal = glm::normalize(normal_nn);
	}
	return normal;
}

void Linear::render(sf::RenderWindow &window) {
	renderPts(window);

	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(start.loc.x, start.loc.y)),
		sf::Vertex(sf::Vector2f(end.loc.x, end.loc.y))
	};
	window.draw(line, 2, sf::Lines);
}

Cubic::Cubic(BezierPoint start, BezierPoint end) : BezierCurve(start, end) {}

glm::vec2 Cubic::getPointAtT(float t) {
	glm::vec2 pos = pow(1.f - t, 3.f) * start.loc + 3.f * pow(1.f - t, 2.f) * t * start.ctrl_loc
		+ 3.f * (1.f - t) * pow(t, 2.f) * end.ctrl_loc + pow(t, 3.f) * end.loc;
	return pos;
}

void Cubic::render(sf::RenderWindow &window) {
	renderPts(window);
	// render the curve
	std::vector<sf::Vertex> line(STEPS + 1);

	for (int step = 0; step < STEPS + 1; step++) {
		float t = step * (1.f / (float)STEPS);
		glm::vec2 pos = getPointAtT(t);
		line[step] = sf::Vertex(sf::Vector2f(pos.x, pos.y));
	}

	window.draw(line.data(), STEPS + 1, sf::LineStrip);
}

float Cubic::getLength() {
	if (this->length == 0) {
		this->length = getLengthAtT(1.f);
	}
	return this->length;
}

float Cubic::getLengthAtT(float tstop) {
	float lenAtT = 0;
	glm::vec2 lastPos = start.loc;
	bool done = false;
	for (int step = 1; !done && step < STEPS + 1; step++) {
		float t = step * (1.f / (float)STEPS);
		if (t > tstop) {
			t = tstop;
			done = true;
		}
		glm::vec2 pos = getPointAtT(t);
		lenAtT += glm::distance(pos, lastPos);
		lastPos = pos;
	}
	return lenAtT;
}

// like binary search
glm::vec2 Cubic::getPointAtLength(float stoplength) {
	float len = 0;
	float dlen = 0;
	float step_size = 1.f / (float)STEPS; // in terms of t
	float t = 0.f; // eventually becomes the t corresponding to stoplength
	glm::vec2 lastPos = start.loc;

	for (int step = 1; step < STEPS + 1; step++) {
		t = step * step_size;
		glm::vec2 pos = getPointAtT(t);
		dlen = glm::distance(pos, lastPos);
		len += dlen;
		if (len > stoplength) {
			// get fraction of excess length
			float frac = (len - stoplength) / dlen;
			// convert it to fraction of t
			t -= frac * step_size;
			break;
		}
		lastPos = pos;
	}

	return getPointAtT(t);
}

glm::vec2 Cubic::getNormalAtT(float t, bool cw) {
	glm::vec2 normalAtT_nn = 6.f * (1.f - t) * (end.ctrl_loc - 2.f *start.ctrl_loc + start.loc)
		+ 6.f * t * (end.loc - 2.f*end.ctrl_loc + start.ctrl_loc);
	if (!cw) {
		normalAtT_nn *= -1.f;
	}
	return glm::normalize(normalAtT_nn);
}