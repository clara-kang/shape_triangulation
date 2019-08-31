#pragma once
#include "Triangulation.hpp"
#include <algorithm>
#include "MathUtil.hpp"

static float PI = 3.1415926f;
static float BIG = 500.f;
static float EPSILON(1e-2);

void Triangulation::render(sf::RenderWindow *window, CONN_T trgltn, const PS_T &P) {
	for (auto it = P.begin(); it != P.end(); ++it) {
		Triangulation::renderPoint(window, *it, sf::Color::Green);
	}
	renderTrngltn(window, trgltn, P);
}

void Triangulation::render(sf::RenderWindow *window, std::vector<int> indices, CONN_T trgltn, const PS_T &P) {
	float POINT_RADIUS = 2.f;
	for (int i = 0; i < indices.size(); i++) {
		//sf::Vector2f pt_loc(P[indices[i]].x - POINT_RADIUS / 2.f, -P[indices[i]].y - POINT_RADIUS / 2.f);
		//sf::CircleShape circle(POINT_RADIUS);
		//circle.setFillColor(sf::Color::Green);
		//circle.setPosition(pt_loc);
		//window->draw(circle);
		Triangulation::renderPoint(window, P[indices[i]], sf::Color::Green);
	}

	renderTrngltn(window, trgltn, P);
}

void Triangulation::renderPoint(sf::RenderWindow *window, glm::vec2 loc, sf::Color col) {
	float POINT_RADIUS = 2.f;

	sf::Vector2f pt_loc(loc.x - POINT_RADIUS / 2.f, -loc.y - POINT_RADIUS / 2.f);
	sf::CircleShape circle(POINT_RADIUS);
	circle.setFillColor(col);
	circle.setPosition(pt_loc);
	window->draw(circle);
}

void Triangulation::renderTrngltn(sf::RenderWindow *window, CONN_T trgltn, const PS_T &P){
	for (auto it = trgltn.begin(); it != trgltn.end(); ++it) {
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(P[it->first].x, -P[it->first].y)),
			sf::Vertex(sf::Vector2f(P[it->second].x, -P[it->second].y))
		};
		window->draw(line, 2, sf::Lines);
	}
}

CONN_T Triangulation::triangulate(sf::RenderWindow *window, const PS_T &P) {
	std::vector<int> indices(P.size());
	for (int i = 0; i < P.size(); i++) {
		indices[i] = i;
	}
	return triangulateRec(window, indices, P);
}

CONN_T Triangulation::triangulateRec(sf::RenderWindow *window, std::vector<int> indices, const PS_T &P) {
	// if equal or less than 3 points, base case
	if (indices.size() <= 3) {
		CONN_T merged;
		int mid_index;
		/*if (indices.size() == 3 && 
			onSameLine(P[indices[0]], P[indices[1]], P[indices[2]], mid_index)) {
			for (int i = 0; i < 3; i++) {
				if (i != mid_index) {
					merged.push_back(std::make_pair(indices[mid_index], indices[i]));
				}
			}
		}
		else {*/
			for (int i = 0; i < indices.size(); i++) {
				for (int j = i + 1; j < indices.size(); j++) {
					merged.push_back(std::make_pair(indices[i], indices[j]));
				}
			}
		//}
		window->clear(sf::Color::Black);
		Triangulation::render(window, indices, merged, P);
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
		std::vector<int> left(indices_sortX.begin(), indices_sortX.begin() + mid_index);
		std::vector<int> right(indices_sortX.begin() + mid_index, indices_sortX.begin()+ indices_sortX.size());
		CONN_T left_trgltn = triangulateRec(window, left, P);
		CONN_T right_trgltn = triangulateRec(window, right, P);

		window->clear();
		render(window, indices, left_trgltn, P);
		render(window, indices, right_trgltn, P);
		window->display();

		CONN_T merged = mergeLeftRight(window, left_trgltn, right_trgltn, left, right, P);

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
			angle = getSignedAngle(to_cand, base_v);
		}
		else {
			angle = getSignedAngle(base_v, to_cand);
		}
		angle = fmod(angle + 2.f * PI, 2.f * PI);
		angles_w_base.push_back(std::make_pair(cnddtes[i], angle));
	}
	if (angles_w_base.size() > 0) {
		// sort angles from small to big
		auto min_angle = [](std::pair<int, float> &a1, std::pair<int, float> &a2) {
			return a1.second < a2.second;
		};
		std::sort(angles_w_base.begin(), angles_w_base.end(), min_angle);

		int valid_right_cand = -1;
		// for each candidate, find center of circle left-base, right-base, cand
		for (int i = 0; i < angles_w_base.size(); i++) {
			// last candidate
			if (i == angles_w_base.size() - 1) {
				bool angle_valid = angles_w_base[i].second < PI - EPSILON;
				if (angle_valid) {
					valid_right_cand = angles_w_base[i].first;
				}
				break;
			}
			// same line, continue
			if (angles_w_base[i].second < EPSILON || angles_w_base[i].second > 2.f * PI - EPSILON) {
				continue;
			}
			// angle bigger than 180, no valid candidate
			else if (angles_w_base[i].second >= PI - EPSILON) {
				break;
			}
			int cand_index = angles_w_base[i].first;
			const glm::vec2 &cand = P[cand_index];
			const glm::vec2 &next_cand = P[angles_w_base[i + 1].first];
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

std::pair<int, int> Triangulation::findBaseEdge(CONN_T left_trgltn, CONN_T right_trgltn,
	std::vector<int> left_indices, std::vector<int> right_indices, const PS_T &P, sf::RenderWindow *window) {
	auto minY = [P](int i1, int i2) {
		return P[i1].y < P[i2].y;
	};
	// get lowest pt of left and right part
	int left_minY_index = *(std::min_element(left_indices.begin(), left_indices.end(), minY));
	int right_minY_index = *(std::min_element(right_indices.begin(), right_indices.end(), minY));

	// left triangle has lowest vertex
	std::vector<int> lower_vs;
	if (P[left_minY_index].y < P[right_minY_index].y) {
		// get all vertices lower than right_minY_index
		for (auto it = left_indices.begin(); it != left_indices.end(); ++it) {
			if (P[*it].y < P[right_minY_index].y) {
				lower_vs.push_back(*it);
			}
		}
		std::sort(lower_vs.begin(), lower_vs.end(), minY);
		// location of right lowest v
		glm::vec2 right_base_v = P[right_minY_index];
		// start from lowest
		for (int i = 0; i < lower_vs.size(); i++) {
			glm::vec2 left_base_v = P[lower_vs[i]];
			bool intrsct = false;
			for (auto it = left_trgltn.begin(); it != left_trgltn.end(); ++it) {
				// todo:: remove after debug
				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(P[it->first].x, -P[it->first].y), sf::Color::Blue),
					sf::Vertex(sf::Vector2f(P[it->second].x, -P[it->second].y), sf::Color::Blue)
				};
				window->clear();
				render(window, left_indices, left_trgltn, P);
				render(window, right_indices, right_trgltn, P);
				renderPoint(window, P[lower_vs[i]], sf::Color::Yellow);
				renderPoint(window, P[right_minY_index], sf::Color::Blue);
				window->draw(line, 2, sf::Lines);
				window->display();
				// end of rendering
				if (segIntersectMiddle(P[it->first], P[it->second], right_base_v, left_base_v)) {
					intrsct = true;
					break;
				}
			}
			if (!intrsct) {
				left_minY_index = lower_vs[i];
				break;
			}
			// reached the last one, still didn't find a base vertex
			else if (i == lower_vs.size() - 1) {
				//throw std::exception("didn't find base v");
				left_minY_index = lower_vs[i];
				break;
			}
		}
	}
	else {
		// get all vertices lower than left_minY_index
		for (auto it = right_indices.begin(); it != right_indices.end(); ++it) {
			if (P[*it].y < P[left_minY_index].y) {
				lower_vs.push_back(*it);
			}
		}
		std::sort(lower_vs.begin(), lower_vs.end(), minY);
		// location of left lowest v
		glm::vec2 left_base_v = P[left_minY_index];
		// start from lowest
		for (int i = 0; i < lower_vs.size(); i++) {
			glm::vec2 right_base_v = P[lower_vs[i]];
			bool intrsct = false;
			for (auto it = right_trgltn.begin(); it != right_trgltn.end(); ++it) {
				// todo:: remove after debug
				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(P[it->first].x, -P[it->first].y), sf::Color::Blue),
					sf::Vertex(sf::Vector2f(P[it->second].x, -P[it->second].y), sf::Color::Blue)
				};
				window->clear();
				render(window, left_indices, left_trgltn, P);
				render(window, right_indices, right_trgltn, P);
				renderPoint(window, P[left_minY_index], sf::Color::Yellow);
				renderPoint(window, P[lower_vs[i]], sf::Color::Blue);
				window->draw(line, 2, sf::Lines);
				window->display();
				// end of rendering
				if (segIntersectMiddle(P[it->first], P[it->second], left_base_v, right_base_v)) {
					intrsct = true;
					break;
				}
			}
			if (!intrsct) {
				right_minY_index = lower_vs[i];
				break;
			}
			// reached the last one, still didn't find a base vertex
			else if (i == lower_vs.size() - 1) {
				// todo :: handle same line
				//throw std::exception("didn't find base v");
				right_minY_index = lower_vs[i];
				break;
			}
		}
	}
	return std::make_pair(left_minY_index, right_minY_index);
}

CONN_T Triangulation::mergeLeftRight(sf::RenderWindow *window, CONN_T left_trgltn, CONN_T right_trgltn,
	std::vector<int> left_indices, std::vector<int> right_indices, const PS_T &P) {

	auto base_edge_info = findBaseEdge(left_trgltn, right_trgltn, left_indices, right_indices, P, window);
	int left_minY_index = base_edge_info.first;
	int right_minY_index = base_edge_info.second;

	// merged conn list
	CONN_T merged;
	// push the lowest edge
	merged.push_back(std::make_pair(left_minY_index, right_minY_index));
	int left_last_index = left_minY_index;
	int right_last_index = right_minY_index;

	while (true) {
		window->clear();
		render(window, left_indices, left_trgltn, P);
		render(window, right_indices, right_trgltn, P);
		render(window, right_indices, merged, P);
		renderPoint(window, P[left_last_index], sf::Color::Yellow);
		renderPoint(window, P[right_last_index], sf::Color::Blue);
		window->display();

		int right_cand = getLeftRightCandidate(true, left_last_index, right_last_index, right_trgltn, P);
		int left_cand = getLeftRightCandidate(false, right_last_index, left_last_index, left_trgltn, P);

		bool use_right_cand = false;
		bool two_cands = false;
		// if both right and left candidate submitted
		if (right_cand >= 0 && left_cand >= 0) {
			two_cands = true;
			// check right_can in circle base-edge, left-cand
			auto circle_info = getCircleFrom3Points(P[left_last_index], P[right_last_index], P[left_cand]);
			glm::vec2 center = circle_info.first;
			float radius = circle_info.second;
			// use right candidate if it's inside circle
			float right_cand_center_dist = glm::distance(center, P[right_cand]);
			use_right_cand = (radius > BIG) || (right_cand_center_dist <= radius);
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
