#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <exception>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <map>
#include "BezierPoint.hpp"
#include "BezierCurve.hpp"
#include "Shape.hpp"
#include "PointUtil.hpp"

enum CurveMode {LINEAR, CUBIC};
enum DrawMode {ON, OFF, MOVE_CTRL, MOVE_BP};

CurveMode curveMode = LINEAR;
DrawMode drawMode = OFF;

int BUTTON_XOFFSET = 30;
int BUTTON_LEN = 50;
int MENU_WIDTH = 25;
float DETECTION_RANGE = 12; // in pixel
float PS_Lm = 20; // distance between points

sf::Text buttonText;

// all the curves
std::vector<BezierCurve*> curvesSoup;
// unfinished curve
std::shared_ptr<BezierPoint> loneBP;
// last mouse clicked position
glm::vec2 lastClickedPos;
// chosen bezier point to change position
BezierPoint* clickedBp;
// chosen curve
BezierCurve* clickedCurve;
// the shapes
std::vector<Shape*> shapesSoup;
// the point sets
std::vector<PointUtil*> pointUtils;
// the map mapping completed shapes to pointUtils
std::map<Shape *, PointUtil*> shape2Points;

void drawMenuBar(sf::RenderWindow &window, sf::Font &font) {
	float window_width = window.getSize().x;
	sf::RectangleShape rect(sf::Vector2f(window_width, MENU_WIDTH));
	rect.setFillColor(sf::Color(50,50,50));
	rect.setPosition(0, 0);

	buttonText.setFont(font);
	if (curveMode == LINEAR) {
		buttonText.setString("Linear");
	}
	else {
		buttonText.setString("Cubic");
	}
	buttonText.setCharacterSize(24);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setPosition(BUTTON_XOFFSET, 0);

	window.draw(rect);
	window.draw(buttonText);
}

bool isCurveModeClicked(int x, int y) {
	return (x >= BUTTON_XOFFSET && x <= BUTTON_XOFFSET + BUTTON_LEN && y <= MENU_WIDTH);
}

bool isMenuClicked(int y) {
	return y < MENU_WIDTH;
}

// helper for isHandleClicked
bool isThisHandleClicked(BezierPoint &bp, glm::vec2 clickedPos) {
	float clickPos2Handle = glm::distance(bp.ctrl_loc, clickedPos);
	return (clickPos2Handle < DETECTION_RANGE);
}

// helper for isBPClicked
bool isThisBPClicked(BezierPoint &bp, glm::vec2 clickedPos) {
	float clickPos2BP = glm::distance(bp.loc, clickedPos);
	return (clickPos2BP < DETECTION_RANGE);
}

// check if any bezier point handle clicked
bool isHandleClicked(glm::vec2 clickedPos) {
	for (std::vector<BezierCurve*>::iterator it = curvesSoup.begin(); it != curvesSoup.end(); ++it) {
		BezierPoint &sBp = (*it)->getStart();
		BezierPoint &eBp = (*it)->getEnd();
		if (sBp.isCubic() && isThisHandleClicked(sBp, clickedPos)) {
			clickedBp = &((*it)->start);
			return true;
		}
		else if (eBp.isCubic() && isThisHandleClicked(eBp, clickedPos)) {
			clickedBp = &((*it)->end);
			return true;
		}
	}
	return false;
}

// check if any bezier point clicked
bool isBPClicked(glm::vec2 clickedPos) {
	for (std::vector<BezierCurve*>::iterator it = curvesSoup.begin(); it != curvesSoup.end(); ++it) {
		if ( isThisBPClicked((*it)->getStart(), clickedPos)) {
			clickedBp = &((*it)->start);
			clickedCurve = (*it);
			return true;
		}
		else if (isThisBPClicked((*it)->getEnd(), clickedPos)) {
			clickedBp = &((*it)->end);
			clickedCurve = (*it);
			return true;
		}
	}
	return false;
}

// get shape containing curve
Shape *getShapeWithCurve(BezierCurve *curve) {
	Shape *shape = NULL;
	for (auto it = shapesSoup.begin(); it != shapesSoup.end(); ++it) {
		if ((*it)->curveInShape(curve)) {
			shape = *it;
			break;
		}
	}
	return shape;
}

// only fuse start to end
bool closeToShape(Shape **touchedShape) {
	bool moveStart = ( clickedBp == &clickedCurve->getStart());
	for (std::vector<Shape*>::iterator it = shapesSoup.begin(); it != shapesSoup.end(); ++it) {
		// if the shape already forms a loop, cannot merge with it
		if ((*it)->completed) {
			continue;
		}
		BezierPoint *bp_to_check;
		// move start of curve, check curve start close to the tail of the shape
		if (moveStart) {
			bp_to_check = (*it)->getTail();
		}
		// move end of curve,  check curve start close to the start of the shape
		else {
			bp_to_check = (*it)->getHead();
		}
		if (glm::distance(clickedBp->loc, bp_to_check->loc) < DETECTION_RANGE) {
			*touchedShape = *it;
			return true;
		}
	}
	return false;
}

// merge shape with shape
void mergeShape(Shape *touchedShape) {
	Shape *shape_to_merge = NULL;
	for (auto it = shapesSoup.begin(); it != shapesSoup.end(); ++it) {
		if ((*it)->curveInShape(clickedCurve)) {
			shape_to_merge = *it;
			// if merging two different curves delete the current one
			if ( shape_to_merge != touchedShape) {
				shapesSoup.erase(it);
			}
			break;
		}
	}
	bool moveStart = (clickedBp == &clickedCurve->getStart());
	if (shape_to_merge != NULL) {
		if (moveStart) {
			// snap
			clickedBp->loc = touchedShape->getTail()->loc;
			touchedShape->mergeToTail(shape_to_merge);
		}
		else {
			// snap
			clickedBp->loc = touchedShape->getHead()->loc;
			touchedShape->mergeToHead(shape_to_merge);
		}
		// delete the merged shape except when connecting head and tail of one shape
		if (shape_to_merge != touchedShape) {
			delete shape_to_merge;
		}
	}
	else {
		throw std::exception("cannot find shape");
	}
}

void drawCurves(sf::RenderWindow &window) {
	for (std::vector<BezierCurve*>::iterator it = curvesSoup.begin(); it != curvesSoup.end(); ++it) {
		(*it)->render(window);
	}
}

void drawPSets(sf::RenderWindow &window) {
	for (auto it = pointUtils.begin(); it != pointUtils.end(); ++it) {
		(*it)->render(window);
	}
}

void redrawEvrything(sf::RenderWindow &window, sf::Font &font) {
	drawMenuBar(window, font);
	drawCurves(window);
	drawPSets(window);
	window.display();
}

int main()
{
	// load font
	sf::Font font;
	if (!font.loadFromFile("../fonts/Gula_FREE.ttf")) {
		throw std::exception("cannot read font");
	}

	sf::RenderWindow  window(sf::VideoMode(800, 600), "My window");
	window.clear(sf::Color::Black);
	drawMenuBar(window, font);
	window.display();

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		// if dragging
		if (drawMode == MOVE_CTRL || drawMode == MOVE_BP) {
			 // mouse released
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				// check if merge shape
				drawMode = OFF;
			}
			else {
				sf::Vector2i position = sf::Mouse::getPosition(window);
				if (drawMode == MOVE_CTRL) {
					clickedBp->ctrl_loc = glm::vec2(position.x, position.y);
				}
				// move point
				else {
					// move point clicked
					clickedBp->moveTo(glm::vec2(position.x, position.y));
					// get shape curve is in
					Shape *shape = getShapeWithCurve(clickedCurve);
					// check if there is a nb in shape that needs to be moved together
					BezierPoint *bp_together = shape->getNbPoint(clickedBp);
					if (bp_together != NULL) {
						bp_together->moveTo(glm::vec2(position.x, position.y));
					}

					Shape *cShape = NULL;
					// if need to merge
					if (closeToShape(&cShape)) {
						mergeShape(cShape);
						// terminate dragging
						drawMode = OFF;
					}
					if (cShape != NULL && cShape->completed) {
						PointUtil *pUtil = new PointUtil(cShape, PS_Lm);
						pUtil->computePm();
						pointUtils.push_back(pUtil);
						shape2Points[cShape] = pUtil;
					}
				}
				// redraw
				window.clear();
				redrawEvrything(window, font);
			}
		} 

		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				window.clear();
				std::cout << "mouse pressed: " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
				// can only click menu is not in the middle of drawing a curve
				if (isMenuClicked(event.mouseButton.y) && (drawMode!= ON)) {
					// curve mode button clicked
					if (isCurveModeClicked(event.mouseButton.x, event.mouseButton.y)) {
						if (curveMode == LINEAR) {
							curveMode = CUBIC;
						}
						else {
							curveMode = LINEAR;
						}
					}
				}
				else {
					glm::vec2 clickedPos = glm::vec2(event.mouseButton.x, event.mouseButton.y); 
					bool cubic = (curveMode == CUBIC);
					// if not in the middle of drawing a curve
					if (drawMode == OFF) {
						// check if any handle chosen
						if (isHandleClicked(clickedPos)) {
							drawMode = MOVE_CTRL;
						}
						// check if any bp chosen
						else if (isBPClicked(clickedPos)) {
							drawMode = MOVE_BP;
						}
						// starting a new curve
						else {
							drawMode = ON;
							loneBP = std::make_shared<BezierPoint>(clickedPos, cubic);
							loneBP->render(window);
						}
					}
					// if in the middle of drawing a curve
					else if (drawMode == ON) {
						drawMode = OFF;
						BezierPoint endBP(clickedPos, cubic);
						BezierCurve *curve;
						if (curveMode == CUBIC) {
							// create cubic curve
							Cubic *cubic = new Cubic(*loneBP, endBP);
							curvesSoup.push_back(cubic);
							curve = cubic;
						}
						else {
							// create line
							Linear *linear = new Linear(*loneBP, endBP);
							curvesSoup.push_back(linear);
							curve = linear;
						}
						// create shape
						Shape *shape = new Shape(curve);
						shapesSoup.push_back(shape);
					}
					lastClickedPos = clickedPos;
				}
				redrawEvrything(window, font);
			}
		}

	}

	return 0;
}