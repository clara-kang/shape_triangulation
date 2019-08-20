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

enum Mode {SELECT, DRAW};
enum CurveMode {LINEAR, CUBIC};
enum DrawMode {ON, OFF, MOVE_CTRL, MOVE_BP};

Mode mode = SELECT;
CurveMode curveMode = LINEAR;
DrawMode drawMode = OFF;

int BUTTON_XOFFSET = 30;
int BUTTON_LEN = 50;
int MENU_WIDTH = 25;
float DETECTION_RANGE = 12; // in pixel
float PS_Lm = 20; // distance between points
bool moveStart; // move start of end of clicked curve
bool selecting = false;;

sf::Text buttonText;
sf::RectangleShape selectionRect;

// all the curves
std::vector<BezierCurve*> curvesSoup;
// unfinished curve
std::shared_ptr<BezierPoint> loneBP;
// last mouse clicked position
glm::vec2 lastClickedPos;
// chosen bezier point to change position
//const BezierPoint* clickedBp;
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

	sf::Text modeText;
	modeText.setFont(font);
	if (mode == SELECT) {
		modeText.setString("Select");
	}
	else {
		modeText.setString("Draw");
	}
	modeText.setCharacterSize(24);
	modeText.setFillColor(sf::Color::White);
	// TODO: use length of prev button
	modeText.setPosition(BUTTON_XOFFSET + BUTTON_LEN, 0);

	window.draw(rect);
	window.draw(buttonText);
	window.draw(modeText);
}

bool isCurveModeClicked(int x, int y) {
	return (x >= BUTTON_XOFFSET && x <= BUTTON_XOFFSET + BUTTON_LEN && y <= MENU_WIDTH);
}

int whichButtonClicked(int x, int y) {
	if (y <= MENU_WIDTH) {
		if (x >= BUTTON_XOFFSET && x <= BUTTON_XOFFSET + BUTTON_LEN) {
			return 0;
		}
		else if (x >= BUTTON_XOFFSET + BUTTON_LEN && x <= BUTTON_XOFFSET + 2.f * BUTTON_LEN) {
			return 1;
		}
	}
	return -1;
}

bool isMenuClicked(int y) {
	return y < MENU_WIDTH;
}

// helper for isHandleClicked
bool isThisHandleClicked(const BezierPoint &bp, glm::vec2 clickedPos) {
	float clickPos2Handle = glm::distance(bp.ctrl_loc, clickedPos);
	return (clickPos2Handle < DETECTION_RANGE);
}

// helper for isBPClicked
bool isThisBPClicked(const BezierPoint &bp, glm::vec2 clickedPos) {
	float clickPos2BP = glm::distance(bp.loc, clickedPos);
	return (clickPos2BP < DETECTION_RANGE);
}

// check if any bezier point handle clicked
bool isHandleClicked(glm::vec2 clickedPos) {
	for (std::vector<BezierCurve*>::iterator it = curvesSoup.begin(); it != curvesSoup.end(); ++it) {
		const BezierPoint &sBp = (*it)->getStart();
		const BezierPoint &eBp = (*it)->getEnd();
		if (sBp.isCubic() && isThisHandleClicked(sBp, clickedPos)) {
			//clickedBp = &sBp;
			clickedCurve = *it;
			moveStart = true;
			return true;
		}
		else if (eBp.isCubic() && isThisHandleClicked(eBp, clickedPos)) {
			//clickedBp = &eBp;
			clickedCurve = *it;
			moveStart = false;
			return true;
		}
	}
	return false;
}

// check if any bezier point clicked
bool isBPClicked(glm::vec2 clickedPos) {
	for (std::vector<BezierCurve*>::iterator it = curvesSoup.begin(); it != curvesSoup.end(); ++it) {
		if ( isThisBPClicked((*it)->getStart(), clickedPos)) {
			//clickedBp = &((*it)->getStart());
			clickedCurve = (*it);
			moveStart = true;
			return true;
		}
		else if (isThisBPClicked((*it)->getEnd(), clickedPos)) {
			//clickedBp = &((*it)->getEnd());
			clickedCurve = (*it);
			moveStart = false;
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
	//moveStart = ( clickedBp == &clickedCurve->getStart());
	glm::vec2 clickedBpPos;
	if (moveStart) {
		clickedBpPos = clickedCurve->getStart().loc;
	}
	else {
		clickedBpPos = clickedCurve->getEnd().loc;
	}
	for (std::vector<Shape*>::iterator it = shapesSoup.begin(); it != shapesSoup.end(); ++it) {
		// if the shape already forms a loop, cannot merge with it
		if ((*it)->completed) {
			continue;
		}
		const BezierPoint *bp_to_check;
		// move start of curve, check curve start close to the tail of the shape
		if (moveStart) {
			bp_to_check = (*it)->getTail();
		}
		// move end of curve,  check curve start close to the start of the shape
		else {
			bp_to_check = (*it)->getHead();
		}
		if (glm::distance(clickedBpPos, bp_to_check->loc) < DETECTION_RANGE) {
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
	if (shape_to_merge != NULL) {
		if (moveStart) {
			// snap
			clickedCurve->moveStartPos(touchedShape->getTail()->loc);
			touchedShape->mergeToTail(shape_to_merge);
		}
		else {
			// snap
			clickedCurve->moveEndPos(touchedShape->getHead()->loc);
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

void drawSelectBox(sf::RenderWindow &window) {
	if (mode == SELECT) {
		window.draw(selectionRect);
	}
}

void redrawEvrything(sf::RenderWindow &window, sf::Font &font) {
	drawMenuBar(window, font);
	drawCurves(window);
	drawPSets(window);
	drawSelectBox(window);
	window.display();
}

int main()
{
	// load font
	sf::Font font;
	if (!font.loadFromFile("../fonts/Gula_FREE.ttf")) {
		throw std::exception("cannot read font");
	}

	// draw the UI
	sf::RenderWindow  window(sf::VideoMode(800, 600), "My window");
	window.clear(sf::Color::Black);
	drawMenuBar(window, font);
	selectionRect.setFillColor(sf::Color(255, 187, 0, 80));
	selectionRect.setOutlineColor(sf::Color(255, 187, 0, 255));
	selectionRect.setOutlineThickness(1.f);
	window.display();

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		// if dragging
		if (mode == SELECT && selecting) {
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				// check if merge shape
				selecting = false;
			}
			else {
				sf::Vector2i position = sf::Mouse::getPosition(window);
				int selectBoxLen = position.x - selectionRect.getPosition().x;
				int selectBoxWidth = position.y - selectionRect.getPosition().y;
				selectionRect.setSize(sf::Vector2f(selectBoxLen, selectBoxWidth));
				// redraw
				window.clear();
				redrawEvrything(window, font);
			}
		}
		else if (drawMode == MOVE_CTRL || drawMode == MOVE_BP) {
			 // mouse released
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				// check if merge shape
				drawMode = OFF;
			}
			else {
				sf::Vector2i position = sf::Mouse::getPosition(window);
				if (drawMode == MOVE_CTRL) {
					if (moveStart) {
						clickedCurve->moveStartCtrl(glm::vec2(position.x, position.y));
					}
					else {
						clickedCurve->moveEndCtrl(glm::vec2(position.x, position.y));
					}
				}
				// move point
				else {
					// move point clicked
					if (moveStart) {
						clickedCurve->moveStartPos(glm::vec2(position.x, position.y));
					}
					else {
						clickedCurve->moveEndPos(glm::vec2(position.x, position.y));
					}
					// get shape curve is in
					Shape *shape = getShapeWithCurve(clickedCurve);
					// check if there is a nb in shape that needs to be moved together
					//BezierPoint *bp_together = shape->getNbPoint(clickedBp);
					if (moveStart) {
						BezierCurve *nb_curve = shape->getPrevCurve(clickedCurve);
						if (nb_curve != NULL) { 
							nb_curve->moveEndPos(glm::vec2(position.x, position.y)); 
						}
					}
					else {
						BezierCurve *nb_curve = shape->getNextCurve(clickedCurve);
						if (nb_curve != NULL) {
							nb_curve->moveStartPos(glm::vec2(position.x, position.y));
						}
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
						pUtil->passWindow(&window);
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
					int buttonIdClicked = whichButtonClicked(event.mouseButton.x, event.mouseButton.y);
					// curve mode
					if (buttonIdClicked == 0) {
						if (curveMode == LINEAR) {
							curveMode = CUBIC;
						}
						else {
							curveMode = LINEAR;
						}
					}
					else if (buttonIdClicked == 1) {
						if (mode == SELECT) {
							mode = DRAW;
						}
						else {
							mode = SELECT;
						}
					}
				}
				else if (mode == SELECT) {
					// draw square
					selecting = true;
					selectionRect.setPosition(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
					selectionRect.setSize(sf::Vector2f(0.f, 0.f));
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