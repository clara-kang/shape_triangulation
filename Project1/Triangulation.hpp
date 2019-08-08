#pragma once
#include "PointUtil.hpp"
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

typedef std::vector<glm::vec2> PS_T;
typedef std::vector<std::pair<int, int>> CONN_T;

namespace Triangulation {
	void render(sf::RenderWindow *window, std::vector<int> indices, CONN_T left_trgltn, const PS_T &P);
	void renderPoint(sf::RenderWindow *window, glm::vec2 loc, sf::Color col);

	void triangulate(sf::RenderWindow *window, const PS_T &P);

	CONN_T triangulateRec(sf::RenderWindow *window, std::vector<int> indices, const PS_T &P);
	CONN_T mergeLeftRight(sf::RenderWindow *window, CONN_T left_trgltn, CONN_T right_trgltn,
		std::vector<int> left_indices, std::vector<int> right_indices, const PS_T &P);
	std::pair<int, int> findBaseEdge(CONN_T left_trgltn, CONN_T right_trgltn,
		std::vector<int> left_indices, std::vector<int> right_indices, const PS_T &P);
};

