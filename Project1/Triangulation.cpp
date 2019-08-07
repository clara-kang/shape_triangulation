#pragma once
#include "Triangulation.hpp"
#include <algorithm>
#include "MathUtil.hpp"

float PI = 3.1415926;

void Triangulation::render(sf::RenderWindow *window, std::vector<int> indices, CONN_T trgltn, const PS_T &P) {
	float POINT_RADIUS = 2.f;
	for (int i = 0; i < indices.size(); i++) {
		sf::Vector2f pt_loc(P[indices[i]].x - POINT_RADIUS / 2.f, P[indices[i]].y - POINT_RADIUS / 2.f);
		sf::CircleShape circle(POINT_RADIUS);
		circle.setFillColor(sf::Color::Green);
		circle.setPosition(pt_loc);
		window->draw(circle);
	}

	for (auto it = trgltn.begin(); it != trgltn.end(); ++it) {
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(P[it->first].x, P[it->first].y)),
			sf::Vertex(sf::Vector2f(P[it->second].x, P[it->second].y))
		};
		window->draw(line, 2, sf::Lines);
	}
}

void Triangulation::triangulate(sf::RenderWindow *window, const PS_T &P) {
	std::vector<int> indices(P.size());
	for (int i = 0; i < P.size(); i++) {
		indices[i] = i;
	}
	triangulateRec(window, indices, P);
}

CONN_T Triangulation::triangulateRec(sf::RenderWindow *window, std::vector<int> indices, const PS_T &P) {
	// if equal or less than 3 points, base case
	if (indices.size() <= 3) {
		CONN_T merged;
		for (int i = 0; i < indices.size(); i++) {
			for (int j = i + 1; j < indices.size(); j++) {
				merged.push_back(std::make_pair(indices[i], indices[j]));
			}
		}
		//window->clear();
		render(window, indices, merged, P);
		window->display();
		return merged;
	}
	// get up-down and left-right span
	auto compY = [P](int i1, int i2) {
		return P[i1].y < P[i2].y;
	};
	auto compX = [P](int i1, int i2) {
		return P[i1].x < P[i2].x;
	};
	std::vector<int> indices_sortX(indices.size());
	std::vector<int> indices_sortY(indices.size());
	std::copy(indices.begin(), indices.end(), indices_sortX.begin());
	std::copy(indices.begin(), indices.end(), indices_sortY.begin());

	std::sort(indices_sortY.begin(), indices_sortY.end(), compY);
	std::sort(indices_sortX.begin(), indices_sortX.end(), compX);

	float spanY = P[indices_sortY.end()[-1]].y - P[indices_sortY[0]].y;
	float spanX = P[indices_sortX.end()[-1]].x - P[indices_sortX[0]].x;

	int mid_index = indices_sortY.size() / 2;
	//// if spans greater in y, split horizontally
	//if (spanY >= spanX) {
	//	std::vector<int> lower(&indices_sortY[0], &indices_sortY[mid_index]);
	//	std::vector<int> upper(&indices_sortY[mid_index + 1], &indices_sortY[indices_sortY.size() - 1]);
	//	CONN_T lower_trgltn= triangulateRec(lower, P);
	//	CONN_T upper_trgltn = triangulateRec(upper, P);
	//}
	//else {
		// split vertically
		std::vector<int> left(&indices_sortX[0], &indices_sortX[mid_index]);
		std::vector<int> right(&indices_sortX[mid_index + 1], &indices_sortX[indices_sortY.size() - 1]);
		CONN_T left_trgltn = triangulateRec(window, left, P);
		CONN_T right_trgltn = triangulateRec(window, right, P);
		CONN_T merged = mergeLeftRight(left_trgltn, right_trgltn, left, right, P);

		window->clear();
		render(window, indices, merged, P);
		window->display();

		return merged;
	//}
}

// get center and radius of circle determined by 3 points
std::pair<glm::vec2, float> getCircleFrom3Points(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3) {
	// [2*(x2-x1) 2*(y2-y1)] * [x] = [x2^2+y2^2-x1^2-y1^2]   
	// [2*(x3-x2) 2*(y3-y2)]   [y]   [x3^2+y3^2-x2^2-y2^2]
	glm::mat2 A(2.f*(p2.x - p1.x), 2.f*(p3.x - p2.x), 2.f*(p2.y - p1.y), 2.f*(p3.y - p2.y));
	glm::vec2 B(pow(glm::length(p2), 2) - pow(glm::length(p1), 2), pow(glm::length(p3), 2) - pow(glm::length(p2), 2));
	glm::vec2 center = glm::inverse(A) * B;
	float radius = glm::distance(center, p1);
	return std::make_pair(center, radius);
}

int getLeftRightCandidate(bool right, int other_last_index, int this_last_index, 
	CONN_T &trgltn, const PS_T &P) {
	// get potential candidates, vertices that this-last-index connects to
	std::vector<int> cnddtes;
	for (auto it = trgltn.begin(); it != trgltn.end(); ++it) {
		if (it->first == this_last_index) {
			cnddtes.push_back(it->second);
		}
		else if (it->second == this_last_index) {
			cnddtes.push_back(it->first);
		}
	}
	// base vector going from right to left
	glm::vec2 base_v = P[other_last_index] - P[this_last_index];
	// angles of right base point with right candidate, with base vector, the first int being the index in right_cnddtes
	std::vector<std::pair<int, float>> angles_w_base;
	for (int i = 0; i < cnddtes.size(); i++) {
		glm::vec2 to_cand = P[cnddtes[i]] - P[this_last_index];
		float angle;
		if (right) {
			angle = getSignedAngle(base_v, to_cand);
		}
		else {
			angle = getSignedAngle(to_cand, base_v);
		}
		angles_w_base.push_back(std::make_pair(i, angle));
	}
	if (angles_w_base.size() > 0) {
		// sort angles from small to big
		auto min_angle = [](std::pair<int, float> &a1, std::pair<int, float> &a2) {
			return a1.second < a2.second;
		};
		std::sort(angles_w_base.begin(), angles_w_base.end(), min_angle);

		int valid_right_cand = -1;
		// for each candidate, find center of circle left-base, right-base, cand
		for (int i = 0; i < angles_w_base.size() - 1; i++) {
			// angle bigger than 180, no valid candidate
			if (angles_w_base[i].second >= 2.f * PI) {
				break;
			}
			int cand_index = angles_w_base[i].first;
			const glm::vec2 &cand = P[cnddtes[cand_index]];
			const glm::vec2 &next_cand = P[cnddtes[angles_w_base[i + 1].first]];
			auto circle_info = getCircleFrom3Points(P[other_last_index], P[this_last_index], cand);
			// check if the circle contains the next candidate
			if (glm::distance(circle_info.first, next_cand) < circle_info.second) {
				// if contains, break edge from right_last_index to candidate
				auto edge_to_rm = std::find_if(trgltn.begin(), trgltn.end(),
					[this_last_index, cand_index](const std::pair<int, int> &edge) {
					return (edge.first == this_last_index && edge.second == cand_index) ||
						(edge.second == this_last_index && edge.first == cand_index); });
				trgltn.erase(edge_to_rm);
			}
			else {
				// the candidate is valid
				valid_right_cand = cand_index;
				break;
			}
		}
		return valid_right_cand;
	}
	return -1;
}

CONN_T Triangulation::mergeLeftRight(CONN_T left_trgltn, CONN_T right_trgltn,
	std::vector<int> left_indices, std::vector<int> right_indices, const PS_T &P) {
	auto minY = [P](int i1, int i2) {
		return P[i1].y < P[i2].y;
	};
	// get lowest pt of left and right part
	int left_minY_index = *(std::min_element(left_indices.begin(), left_indices.end(), minY));
	int right_minY_index = *(std::min_element(right_indices.begin(), right_indices.end(), minY));
	// merged conn list
	CONN_T merged;
	// push the lowest edge
	merged.push_back(std::make_pair(left_minY_index, right_minY_index));
	int left_last_index = left_minY_index;
	int right_last_index = right_minY_index;

	while (true) {
		int right_cand = getLeftRightCandidate(true, left_last_index, right_last_index, right_trgltn, P);
		int left_cand = getLeftRightCandidate(false, right_last_index, left_last_index, left_trgltn, P);

		bool use_right_cand = false;
		bool two_cands = false;
		// if both right and left candidate submitted
		if (right_cand >= 0 && left_cand >= 0) {
			two_cands = true;
			// check right_can in circle base-edge, left-cand
			auto circle_info = getCircleFrom3Points(P[left_last_index], P[right_last_index], P[left_cand]);
			// use right candidate if it's inside circle
			use_right_cand == (glm::distance(circle_info.first, P[right_cand]) <= circle_info.second);
		}
		if ((right_cand >= 0 && left_cand < 0) || (two_cands && use_right_cand)) {
			// insert edge left_last_index -- right_cand
			merged.push_back(std::make_pair(right_cand, left_last_index));
			right_last_index = right_cand;
		}
		else if ((right_cand < 0 && left_cand >= 0) || (two_cands && !use_right_cand)) {
			// insert edge right_last_index -- left_cand
			merged.push_back(std::make_pair(left_cand, right_last_index));
			left_last_index = left_cand;
		}
		// no candidates submitted
		else {
			break;
		}
	}
	// push the remaining edges in right_trgltn and left_trgltn into merged
	merged.insert(merged.end(), right_trgltn.begin(), right_trgltn.end());
	merged.insert(merged.end(), left_trgltn.begin(), left_trgltn.end());
	return merged;
}
