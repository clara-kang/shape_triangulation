#pragma once
#include <glm/glm.hpp>
#include <vector>

float getSignedAngle(glm::vec2 v1, glm::vec2 v2);

glm::vec2 rotate90cw(glm::vec2 v);

glm::vec2 rotate90ccw(glm::vec2 v);

glm::vec2 rotate(glm::vec2 v, float angle);

bool onSameLine(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int &middle_index);

std::vector<int> computeConvexHull(std::vector<glm::vec2> points);

bool segIntersectMiddle(glm::vec2 seg1_start, glm::vec2 seg1_end,
	glm::vec2 seg2_start, glm::vec2 seg2_end);