#pragma once
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class BezierPoint {
public:
	BezierPoint() {};
	BezierPoint(glm::vec2 loc, bool cubic);

	bool isCubic() const;
	void render(sf::RenderWindow &window);
	glm::vec2 loc;
	glm::vec2 ctrl_loc;
private:
	bool cubic = false;
};
