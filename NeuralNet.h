#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <math.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class NeuralNet {

	// edge weights
	struct Weight {
		double weight;
	};

	class Neuron;
	typedef std::vector<Neuron> Layer;
	typedef std::pair<std::pair<int,int>, std::pair<int, int>> Connection; 
	
	// Neuron
	class Neuron {
		public:
			size_t _id;
			Neuron(size_t id) {
				_id = id;
				_bias = 0;
			}
			
			~Neuron() {}

			void setOutputValue(double output) {
				_outputValue = output;	
			}

			double getOutputValue() {
				return _outputValue;
			}

			void feedForward(Layer prevLayer, std::vector<double> weights) {
				double sum = 0.0;
				for(size_t i = 0; i < prevLayer.size(); i++) {
					sum += prevLayer[i].getOutputValue() * weights[i];
				}	
				sum += _bias;
				_outputValue = sum;
			}

		private:
			static double activationFunction(double x) {
				return tanh(x);
			}

			double _outputValue;
			double _bias;
	};

	public:
		NeuralNet(const std::vector<int> &topology, const sf::Vector2u windowSize);
		~NeuralNet();

		void feedForward(const std::vector<double> &inputs);
		void backPropogation(const std::vector<double> &targetValues);
		std::vector<double> getOutputs();

		// visual
		void renderNetwork(sf::RenderWindow* window);
		std::vector<std::vector<sf::Vector2f>> getNetworkPositions() const {
			return _networkPositions;
		}

	private:
		std::vector<Layer> _network;
		std::map<Connection, Weight> weights;
		size_t _numLayers;

		std::vector<std::vector<sf::Vector2f>> _networkPositions;
};
