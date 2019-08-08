#include "MathUtil.hpp"	
#include <math.h>
#include <algorithm>
#include <iostream>

static float EPSILON(1e-5);

float getSignedAngle(glm::vec2 v1, glm::vec2 v2) {
	// somehow it can exceed 1 by a very little amount
	float cosTheta = std::min(glm::dot(v1, v2) / (glm::length(v1) * glm::length(v2)), 1.f);
	float theta = acos(cosTheta);
	glm::mat2 detMat(v1.x, v1.y, v2.x, v2.y);
	float det = glm::determinant(detMat);
	if (det >= 0) {
		return theta;
	}
	else {
		return -theta;
	}
}

glm::vec2 rotate90cw(glm::vec2 v) {
	return glm::vec2(v.y, -v.x);
}

glm::vec2 rotate90ccw(glm::vec2 v) {
	return glm::vec2(-v.y, v.x);
}

glm::vec2 rotate(glm::vec2 v, float angle) {
	glm::vec2 res;
	glm::mat2 trans_matrix(cos(angle), sin(angle), -sin(angle), cos(angle));
	res = trans_matrix * v;
	if (isnan(res.x) || isnan(res.y)) {
		throw std::exception("res is nan");
	}
	return res;
}

std::vector<int> computeConvexHull(std::vector<glm::vec2> points) {
	float PI = 3.14159f;
	auto smallestX = [](const glm::vec2 &p1, const glm::vec2 &p2) {
		return (p1.x < p2.x);
	};
	int minPIndex = std::min_element(points.begin(), points.end(), smallestX) - points.begin();
	std::vector<int> chIndices;
	chIndices.push_back(minPIndex);
	glm::vec2 lastVector(0.f, 1.f); // y vector
	glm::vec2 lastP = points[minPIndex];
	float smlstDist = 0.f; // to get closest point when colinear

	int endPIndex = -1;
	// can definitely not exceed number of points
	for (int i = 0; i < points.size() + 1; i++) {
		float smlstAngle = 2.f * PI;
		for (int j = 0; j < points.size(); j++) {
			if (j == chIndices.end()[-1]) {
				// itself
				continue;
			}
			else {
				// angle between 0 and 2Pi
				float angle = fmod(getSignedAngle(lastVector, points[j] - lastP) + 2.f * PI, 2.f * PI);
				float dist = glm::distance(points[j], lastP);
				if (angle < smlstAngle) {
					endPIndex = j;
					smlstAngle = angle;
					smlstDist = dist;
				}
			}
		}
		if (endPIndex == chIndices[0]) {
			break;
		}
		chIndices.push_back(endPIndex);
		lastP = points[endPIndex];
		lastVector = points[chIndices.end()[-1]] - points[chIndices.end()[-2]];
	}
	return chIndices;
}

bool segIntersectMiddle(glm::vec2 seg1_start, glm::vec2 seg1_end,
	glm::vec2 seg2_start, glm::vec2 seg2_end) {
	glm::vec2 seg1_dir = seg1_end - seg1_start;
	glm::vec2 seg2_dir = seg2_end - seg2_start;
	glm::mat2 A(seg1_dir, -seg2_dir);
	glm::vec2 B = seg2_start - seg1_start;
	glm::vec2 t = glm::inverse(A) * B;
	bool t0_middle = t[0] > EPSILON && t[0] < 1.f - EPSILON;
	bool t1_middle = t[1] > EPSILON && t[1] < 1.f - EPSILON;
	if (t0_middle && t1_middle) {
		return true;
	}
	return false;
}