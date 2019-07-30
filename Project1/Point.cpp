#include "Point.hpp"

float PPOINT_RADIUS = 2.f;
float NORMAL_LENGTH = 8.f;

Point::Point(glm::vec2 loc, Type type) {
	this->loc = loc;
	this->type = type;
}

Point::Point(glm::vec2 loc, glm::vec2 normal, Type type) : Point(loc, type) {
	this->normal = normal;
}

void Point::render(sf::RenderWindow &window) {
	sf::Vector2f pt_loc(this->loc.x - PPOINT_RADIUS / 2.f, this->loc.y - PPOINT_RADIUS / 2.f);

	sf::CircleShape circle(PPOINT_RADIUS);
	circle.setFillColor(sf::Color::Green);
	circle.setPosition(pt_loc);
	window.draw(circle);

	// if normal defined
	if (glm::length(normal) > 0.f) {
		glm::vec2 normal_ep = loc + NORMAL_LENGTH * normal;
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(loc.x, loc.y), sf::Color::Yellow),
			sf::Vertex(sf::Vector2f(normal_ep.x, normal_ep.y), sf::Color::Yellow)
		};
		window.draw(line, 2, sf::Lines);
	}
}