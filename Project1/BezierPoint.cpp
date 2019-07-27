#include "BezierPoint.hpp"

#pragma once
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

float DEFAULT_XOFFSET = 50.f; // in pixel
float CIRCLE_RADIUS = 5.f;
float SQUARE_LEN = 5.f;

BezierPoint::BezierPoint(glm::vec2 loc, bool cubic=false) {
	this->loc = loc;
	this->ctrl_loc = loc + DEFAULT_XOFFSET * glm::vec2(1.f, 0.f);
	this->cubic = cubic;
}

void BezierPoint::render(sf::RenderWindow &window) {
	// need to offset since circle center not drawn where clicked
	sf::Vector2f pt_loc(this->loc.x - CIRCLE_RADIUS / 2.f, this->loc.y - CIRCLE_RADIUS / 2.f);
	sf::Vector2f ctrl_pt_loc(this->ctrl_loc.x - SQUARE_LEN / 2.f, this->ctrl_loc.y - SQUARE_LEN / 2.f);

	sf::CircleShape circle(CIRCLE_RADIUS);
	circle.setFillColor(sf::Color::White);
	circle.setPosition(pt_loc);
	window.draw(circle);

	if (this->cubic) {
		sf::RectangleShape rect(sf::Vector2f(SQUARE_LEN, SQUARE_LEN));
		rect.setFillColor(sf::Color::White);
		rect.setPosition(ctrl_pt_loc);
		window.draw(rect);
		// draw line between pt and ctrl_pt
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(this->loc.x, this->loc.y)),
			sf::Vertex(sf::Vector2f(this->ctrl_loc.x, this->ctrl_loc.y))
		};
		window.draw(line, 2, sf::Lines);
	}
}


