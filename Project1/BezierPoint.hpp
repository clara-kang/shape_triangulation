#pragma once
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class BezierPoint {
public:
	BezierPoint() {};
	BezierPoint(glm::vec2 loc, bool cubic);
	BezierPoint (const BezierPoint& b);
	BezierPoint &operator=(const BezierPoint& b);

	bool isCubic() const;
	// move loc and ctrl
	void moveTo(glm::vec2 pos);
	void render(sf::RenderWindow &window);
	glm::vec2 loc;
	glm::vec2 ctrl_loc;
private:
	bool cubic = false;
};
