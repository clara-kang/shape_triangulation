#include "Point.hpp"

float PPOINT_RADIUS = 2.f;

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
}