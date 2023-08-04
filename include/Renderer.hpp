#pragma once

#include <cassert>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace sfml_ren {

class Renderer{

	public:
		Renderer(sf::RenderWindow& window) :
		_window(window)
		{
			_font = new sf::Font();
			if(!_font->loadFromFile("include/util/NotoSans-ReRl.ttf")){
				std::cout << "font did not load" << std::endl;
			}
			_neuronOutput = sf::Text();
			_neuronOutput.setFont(*_font);
			_neuronOutput.setCharacterSize(16);
			_neuronOutput.setFillColor(sf::Color::White);
			/*
			_connectionWeight = sf::Text();
			_connectionWeight.setFont(*_font);
			_connectionWeight.setCharacterSize(16);
			_connectionWeight.setFillColor(sf::Color::White);
			*/
		}

		~Renderer() {
			delete _font;
		}

		void renderNetwork(std::vector<std::vector<sf::Vector2f>> networkPositions,
			std::vector<double> networkWeights,
			std::vector<double> neuronOutputs){

			bool realOutput = false;
			if(neuronOutputs.size() != 0) {
				realOutput = true;
			}

			// create neuron shape
			sf::CircleShape neuronShape = sf::CircleShape();
			float radius = 30;
			neuronShape.setRadius(radius);
			neuronShape.setFillColor(sf::Color::Black);
			neuronShape.setOutlineThickness(3);
			neuronShape.setOutlineColor(sf::Color::White);
		
			// int weightNum = 0;
			int neuronNum = 0;

			// iterate through network topology
			for(size_t i = 0; i < networkPositions.size(); i++) {
				for(size_t k = 0; k < networkPositions[i].size(); k++) {
					// set neuron shape position & text 
					neuronShape.setPosition(networkPositions[i][k]);
					if(realOutput) {
						std::ostringstream neuronOutput;
						neuronOutput << std::fixed;
						neuronOutput << std::setprecision(2);
						neuronOutput << neuronOutputs[neuronNum];
						_neuronOutput.setString(neuronOutput.str());
					} else {
						_neuronOutput.setString("N/A");
					}
					_neuronOutput.setPosition(
						networkPositions[i][k].x + radius - 
						(_neuronOutput.getGlobalBounds().width)/2,
						networkPositions[i][k].y + radius - 
						(_neuronOutput.getGlobalBounds().height)/2
					);
					neuronNum ++;

					// check that neuron is not in the output layer
					if(i+1 != networkPositions.size()) {	
						// create a line (weight) from current neuron to neuron in next layer
						for(size_t j = 0; j < networkPositions[i+1].size(); j++) {
							
							sf::Vertex line[] = {
								sf::Vertex(sf::Vector2f(networkPositions[i][k].x + radius,
									networkPositions[i][k].y + radius), sf::Color::White),
								sf::Vertex(sf::Vector2f(networkPositions[i+1][j].x + radius, 
									networkPositions[i+1][j].y + radius), sf::Color::White),
							};
							_window.draw(line, 2, sf::Lines);
						
							// TODO drawing the weights of all connections may be difficult,
							// set position & text of _connection weight
							/*
							float xWeightPos = (networkPositions[i][k].x + 
								networkPositions[i+1][j].x)/2;
							float yWeightPos = (networkPositions[i][k].y + 
								networkPositions[i+1][j].y)/2;
							std::string text = std::to_string(networkWeights[weightNum]);
							_connectionWeight.setString(text);
							_connectionWeight.setPosition(xWeightPos, yWeightPos);
							std::cout << j << ": " << xWeightPos << ", " << yWeightPos << std::endl;
							// _window.draw(_connectionWeight);
							weightNum ++;*/
						}
					}
					_window.draw(neuronShape);
					_window.draw(_neuronOutput);
				}
			}
		}
	
	private:
		sf::Font* _font;
		sf::Text _neuronOutput;
		sf::Text _connectionWeight;
		sf::RenderWindow& _window;
};

}
