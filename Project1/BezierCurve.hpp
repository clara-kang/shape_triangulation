#pragma once
#include <glm/glm.hpp>
#include <math.h> 
#include <vector>
#include "BezierPoint.hpp"

int STEPS = 10;

class BezierCurve {
private:
	virtual glm::vec2 getStart() = 0;
	virtual glm::vec2 getEnd() = 0;
	virtual void render(sf::RenderWindow &window) = 0;
};

class Linear : public BezierCurve {
public:
	Linear(glm::vec2 start_loc, glm::vec2 end_loc) {
		this->start_loc = start_loc;
		this->end_loc = end_loc;
	}

	glm::vec2 getStart() {
		return start_loc;
	};
	
	glm::vec2 getEnd() {
		return end_loc;
	}
	glm::vec2 start_loc;
	glm::vec2 end_loc;
};

class Cubic : public BezierCurve {
public:
	BezierPoint start;
	BezierPoint end;

	Cubic(BezierPoint start, BezierPoint end) {
		this->start = start;
		this->end = end;
	}

	glm::vec2 getStart() {
		return start.loc;
	};

	glm::vec2 getEnd() {
		return end.loc;
	}

	void render(sf::RenderWindow &window) {
		start.render(window);
		end.render(window);
		// render the curve
		std::vector<sf::Vertex> line(STEPS + 1);

		for (int step = 0; step < STEPS+1; step++) {
			float t = step * (1.f / (float)STEPS);
			glm::vec2 pos = pow(1.f - t, 3.f) * start.loc + 3.f * pow(1.f - t, 2.f) * t * start.ctrl_loc
				+ 3.f * (1.f - t) * pow(t, 2.f) * end.ctrl_loc + pow(t, 3.f) * end.loc;
			line[step] = sf::Vertex(sf::Vector2f(pos.x, pos.y));
		}

		window.draw(line.data(), STEPS+1, sf::LineStrip);
	}
};