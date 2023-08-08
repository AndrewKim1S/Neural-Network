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

			double getGradient() {
				return _gradient;
			}

			void changeBias(double b) {
				_bias += b;
			}

			void feedForward(Layer& prevLayer, std::vector<double>& weights) {
				double sum = 0.0;
				for(size_t i = 0; i < prevLayer.size(); i++) {
					sum += prevLayer[i].getOutputValue() * weights[i];
				}	
				sum += _bias;
				_outputValue = activationFunction(sum); // add activation function
			}

			void calculateOutputGradient(double target) {
				double delta = target - _outputValue;
				_gradient = delta * activationFunctionDerivative(_outputValue); // the direction
			}

			void calculateHiddenGradient(double sum) {
				_gradient = sum * activationFunctionDerivative(_outputValue);
			}

		private:
			static double activationFunction(double x) {
				// return tanh(x);
				return 1.0 / (1.0 + exp(-x)); // sigmoid
			}
			static double activationFunctionDerivative(double x) {
				return x * (1.0 - x);
			}

			double _outputValue;
			double _bias;
			double _gradient;
	};

	public:
		NeuralNet(const std::vector<int> &topology, const sf::Vector2u windowSize);
		~NeuralNet();

		void feedForward(const std::vector<double> &inputs);
		void backPropogation(const std::vector<double> &targetValues);
		std::vector<double> getOutputs();

		// visual & other
		std::vector<std::vector<sf::Vector2f>> getNetworkPositions() const {
			return _networkPositions;
		}
		std::vector<double> getNetworkWeights() {
			_networkWeights.clear();
			for(size_t i = 0; i < _network.size(); i++) {
				for(size_t k = 0; k < _network[i].size(); k++) {
					if(i+1 != _network.size()) {
						for(size_t j = 0; j < _network[i+1].size(); j++) {
							std::pair<int, int> from {i, k};
							std::pair<int, int> to {i+1, j};
							auto w = _weights.find(Connection(from, to));
							_networkWeights.push_back(w->second.weight);
						}
					}
				}
			}
			return _networkWeights;
		}

		std::vector<double> getAllOutputs() const {
			return allNeuronOutputs;
		}

	private:
		std::vector<Layer> _network;
		std::map<Connection, Weight> _weights;
		size_t _numLayers;

		double _learningRate = 0.1;

		std::vector<std::vector<sf::Vector2f>> _networkPositions;
		std::vector<double> _networkWeights;
		std::vector<double> allNeuronOutputs;
};
