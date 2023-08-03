#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cassert>

namespace sfml_ren {

class Renderer{

	public:
		Renderer(sf::RenderWindow& window) :
		_window(window)
		{
			_font = new sf::Font();
			if(!_font->loadFromFile("util/NotoSans-ReRl.ttf")){
				std::cout << "font did not load" << std::endl;
			}
			_number = sf::Text();
			_number.setFont(*_font);
			_number.setCharacterSize(12);
			_number.setFillColor(sf::Color::White);
		}

		~Renderer() {
			delete _font;
		}

		void renderNetwork(std::vector<std::vector<sf::Vector2f>> _networkPositions){
			// render network topology
			sf::CircleShape neuronShape = sf::CircleShape();
			float radius = 30;
			neuronShape.setRadius(radius);
			neuronShape.setFillColor(sf::Color::Black);
			neuronShape.setOutlineThickness(3);
			neuronShape.setOutlineColor(sf::Color::White);
			
			for(size_t i = 0; i < _networkPositions.size(); i++) {
				for(size_t k = 0; k < _networkPositions[i].size(); k++) {
					neuronShape.setPosition(_networkPositions[i][k]);
					if(i+1 != _networkPositions.size()) {	
						for(size_t j = 0; j < _networkPositions[i+1].size(); j++) {
							sf::Vertex line[] = {
								sf::Vertex(sf::Vector2f(_networkPositions[i][k].x + radius, _networkPositions[i][k].y + radius), sf::Color::White),
								sf::Vertex(sf::Vector2f(_networkPositions[i+1][j].x + radius, _networkPositions[i+1][j].y + radius), sf::Color::White),
							};
							_window.draw(line, 2, sf::Lines);
						}
					}
					_window.draw(neuronShape);
				}
			}
		}
	
	private:
		sf::Font* _font;
		sf::Text _number;
		sf::RenderWindow& _window;
};

}
