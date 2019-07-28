#pragma once
#include <glm/glm.hpp>
#include <math.h> 
#include <vector>
#include "BezierPoint.hpp"
#include "BezierCurve.hpp"

int STEPS = 10;
float POINT_RADIUS = 5.f;

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

void Cubic::render(sf::RenderWindow &window) {
	renderPts(window);
	// render the curve
	std::vector<sf::Vertex> line(STEPS + 1);

	for (int step = 0; step < STEPS + 1; step++) {
		float t = step * (1.f / (float)STEPS);
		glm::vec2 pos = pow(1.f - t, 3.f) * start.loc + 3.f * pow(1.f - t, 2.f) * t * start.ctrl_loc
			+ 3.f * (1.f - t) * pow(t, 2.f) * end.ctrl_loc + pow(t, 3.f) * end.loc;
		line[step] = sf::Vertex(sf::Vector2f(pos.x, pos.y));
	}

	window.draw(line.data(), STEPS + 1, sf::LineStrip);
}