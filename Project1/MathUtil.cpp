#include "MathUtil.hpp"	
#include <math.h>
#include <algorithm>
#include <iostream>

static float EPSILON(1e-2);
static float PI(3.1415926f);

float getSignedAngle(glm::vec2 v1, glm::vec2 v2) {
	// somehow it's abs value can exceed 1 by a very little amount
	float cosTheta = std::min(glm::dot(v1, v2) / (glm::length(v1) * glm::length(v2)), 1.f);
	cosTheta = std::max(cosTheta, -1.f);

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

bool approx(float num, float aprox_num) {
	return num > aprox_num - EPSILON && num < aprox_num + EPSILON;
}

bool segOverlap(glm::vec2 seg1_start, glm::vec2 seg1_end,
	glm::vec2 seg2_start, glm::vec2 seg2_end) {
	std::vector<glm::vec2> points({ seg1_start, seg1_end, seg2_start, seg2_end });
	std::vector<int> indicesX({ 0, 1, 2, 3 });
	std::vector<int> indicesY({ 0, 1, 2, 3 });
	auto compX = [points](int indx1, int indx2) {return points[indx1].x < points[indx2].x; };
	auto compY = [points](int indx1, int indx2) {return points[indx1].y < points[indx2].y; };
	std::sort(indicesX.begin(), indicesX.end(), compX);
	std::sort(indicesY.begin(), indicesY.end(), compY);
	float spanX = points[indicesX[3]].x - points[indicesX[0]].x;
	float spanY = points[indicesY[3]].y - points[indicesY[0]].y;
	// more accurate with bigger span 
	if (spanX > spanY) {
		// 1&2, 3&4 must be together for segs to not overlap
		if (abs(indicesX[1] - indicesX[0]) == 1 && abs(indicesX[3] - indicesX[2]) == 1) {
			return false;
		}
		// examine the amount of intersection, if small, consider them not overlapped
		else if (glm::length(points[indicesX[2]] - points[indicesX[1]]) < EPSILON) {
			return false;
		}
		return true;
	}
	else {
		if (abs(indicesY[1] - indicesY[0]) == 1 && abs(indicesY[3] - indicesY[2]) == 1) {
			return false;
		}
		else if (glm::length(points[indicesY[2]] - points[indicesY[1]]) < EPSILON) {
			return false;
		}
		return true;
	}
}

bool segIntersectMiddle(glm::vec2 seg1_start, glm::vec2 seg1_end,
	glm::vec2 seg2_start, glm::vec2 seg2_end) {
	glm::vec2 seg1_dir = seg1_end - seg1_start;
	glm::vec2 seg2_dir = seg2_end - seg2_start;
	glm::mat2 A(seg2_dir, -seg1_dir);
	glm::vec2 B = seg1_start - seg2_start;

	float seg_angle = fmod(getSignedAngle(seg1_dir, seg2_dir) + 2.f*PI, 2.f*PI);
	// check if segs parallel
	if (approx(seg_angle, 0.f) || approx(seg_angle, PI) || approx(seg_angle, 2.f*PI)) {
		glm::vec2 seg1_to_seg2 = seg2_start - seg1_end;
		float angle = fmod(getSignedAngle(seg1_dir, seg1_to_seg2) + 2.f*PI, 2.f*PI);
		// check if segs on the same line
		if (approx(abs(angle), PI) || approx(abs(angle), 0.f) || approx(abs(angle), 2.f * PI)) {
			return segOverlap(seg1_start, seg1_end, seg2_start, seg2_end);
		}
		return false;
	}

	// if not parallel
	glm::vec2 t = glm::inverse(A) * B;
	if (isnan(t.x) || isnan(t.y)) {
		throw std::exception("intersection NAN");
	}
	bool t0_middle = t.x > EPSILON && t.x < 1.f - EPSILON;
	bool t1_middle = t.y > EPSILON && t.y < 1.f - EPSILON;
	//bool t0_touch = t[0] >= EPSILON && t[0] <= 1.f - EPSILON;
	//bool t1_touch = t[1] >= EPSILON && t[1] <= 1.f - EPSILON;
	//if ((t0_middle && t1_touch) || (t1_middle && t0_touch)) {
	//	return true;
	//}
	if (t1_middle && t0_middle) {
		return true;
	}
	return false;
}

bool onSameLine(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int &middle_index) {
	glm::vec2 v1 = p2 - p1;
	glm::vec2 v2 = p3 - p1;
	float angle = getSignedAngle(v1, v2);
	if (abs(angle) < EPSILON) {
		// p3 in the middle
		if (glm::length(v1) > glm::length(v2)) {
			middle_index = 2;
		}
		// p2 in the middle
		else {
			middle_index = 1;
		}
		return true;
	}
	// p1 in the middle
	else if (abs(angle) >= PI - EPSILON && abs(angle) <= PI + EPSILON) {
		middle_index = 0;
		return true;
	}
	return false;
}