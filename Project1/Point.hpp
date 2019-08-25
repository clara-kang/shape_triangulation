#pragma once
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Point {
public:
	enum class Type {
		VERTEX, EDGE, INTERNAL
	};
	glm::vec2 loc;
	glm::vec2 normal;
	Type type;

	Point() {}

	Point(glm::vec2 loc, Type type);

	Point(glm::vec2 loc, glm::vec2 normal, Type type);

	Point(const Point &p);

	void render(sf::RenderWindow &window);

};