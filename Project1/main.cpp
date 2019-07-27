#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <exception>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "BezierPoint.hpp"
#include "BezierCurve.hpp"

enum CurveMode {LINEAR, CUBIC};
enum DrawMode {ON, OFF};

CurveMode curveMode = LINEAR;
DrawMode drawMode = OFF;

int BUTTON_XOFFSET = 30;
int BUTTON_LEN = 50;
int MENU_WIDTH = 25;

sf::Text buttonText;

std::vector<BezierCurve*> curvesSoup;
std::shared_ptr<BezierPoint> loneBP;
glm::vec2 lastClickedPos;

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

void drawCurves(sf::RenderWindow &window) {
	for (std::vector<BezierCurve*>::iterator it = curvesSoup.begin(); it != curvesSoup.end(); ++it) {
		(*it)->render(window);
	}
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
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				std::cout << "mouse pressed: " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
				if (isMenuClicked(event.mouseButton.y)) {
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
					if (drawMode == OFF) {
						drawMode = ON;
						loneBP = std::make_shared<BezierPoint>(clickedPos, cubic);
						loneBP->render(window);
					}
					else {
						drawMode = OFF;
						BezierPoint endBP(clickedPos, cubic);
						/*endBP.render(window);*/
						if (curveMode == CUBIC) {
							// create cubic curve
							Cubic *cubic = new Cubic(*loneBP, endBP);
							curvesSoup.push_back(cubic);
						}
						else {
							// create line
							Linear *linear = new Linear(*loneBP, endBP);
							curvesSoup.push_back(linear);
						}
					}
					lastClickedPos = clickedPos;
				}
				drawMenuBar(window, font);
				drawCurves(window);
				window.display();
			}
		}

	}

	return 0;
}