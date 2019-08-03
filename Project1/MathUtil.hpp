#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>

inline float getSignedAngle(glm::vec2 v1, glm::vec2 v2) {
	float cosTheta = glm::dot(v1, v2) / (glm::length(v1) * glm::length(v2));
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

inline glm::vec2 rotate90cw(glm::vec2 v) {
	return glm::vec2(v.y, -v.x);
}

inline glm::vec2 rotate90ccw(glm::vec2 v) {
	return glm::vec2(-v.y, v.x);
}

inline glm::vec2 rotate(glm::vec2 v, float angle) {
	glm::vec2 res;
	glm::mat2 trans_matrix(cos(angle), sin(angle), -sin(angle), cos(angle));
	res = trans_matrix * v;
	if (isnan(res.x)|| isnan(res.y)) {
		throw std::exception("res is nan");
	}
	return res;
}

inline std::vector<int> computeConvexHull(std::vector<glm::vec2> points) {
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
				bool cond1 = angle < smlstAngle;
				// or collinear but closest
				bool cond2 = (angle == smlstAngle && dist <= smlstDist);
				// not already on CH
				//bool cond3 = std::find(chIndices.begin(), chIndices.end(), j) == chIndices.end();
				if (cond2) {
					std::cout << "collinear" << std::endl;
				}
				if ((cond1 || cond2)) {
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