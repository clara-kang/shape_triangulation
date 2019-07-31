#pragma once
#include <glm/glm.hpp>
#include <math.h>

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