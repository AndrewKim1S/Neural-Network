#include "NeuralNet.h"

// Constructor
NeuralNet::NeuralNet(const std::vector<int> &topology, const sf::Vector2u windowSize) {
	// Create network
	_numLayers = topology.size();
	size_t neuronId = 0;
	for(size_t layerNum = 0; layerNum < _numLayers; layerNum++) {
		_network.push_back(Layer());
		for(int neuronNum = 0; neuronNum < topology[layerNum]; neuronNum++) {
			_network[layerNum].push_back(Neuron(neuronId));
			neuronId ++;
		}
	}

	// Randomize Weights
	for(size_t i = 0; i < _network.size(); i++) {
		for(size_t k = 0; k < _network[i].size(); k++) {
			if(i+1 != _network.size()) {
				for(size_t j = 0; j < _network[i+1].size(); j++) {
					std::pair<int, int> from {i, k};
					std::pair<int, int> to {i+1, j};
					Weight w;
					w.weight = ((double)rand()) / RAND_MAX;
					//
					_networkWeights.push_back(w.weight);
					_weights[std::pair<std::pair<int,int>, std::pair<int, int>>(from, to)] = w;
				}
			}
		}
	}

	// Neuron rendering positions
	float radius = 40;
	float neuronWidth = 80.f + radius;
	float layerWidth = 120 + radius;
	float xPos = (windowSize.x //- (windowSize.x * 0.3) 
			- (layerWidth * _network.size()-1) - (2*radius * _network.size()))/2;
	for(size_t layer = 0; layer < _numLayers; layer++) {
		float yPos = (windowSize.y - (neuronWidth*(_network[layer].size()-1)) 
			- (radius * _network[layer].size()))/2;
		xPos += layerWidth;
		_networkPositions.push_back(std::vector<sf::Vector2f>());
		for(size_t n = 0; n < _network[layer].size(); n++) {
			_networkPositions[layer].push_back(sf::Vector2f(xPos, yPos));
			yPos += neuronWidth;
		}
	}
}

void NeuralNet::feedForward(const std::vector<double> &inputs) {
	std::vector<double> outputs;
	// Pass inputs directly as the input layer's outputs
	for(size_t i = 0; i < _network[0].size(); i++) {
		_network[0][i].setOutputValue(inputs[i]);
		outputs.push_back(_network[0][i].getOutputValue());
	}
	// propogate forward for the hidden & output layers
	for(size_t i = 1; i < _network.size(); i++) {
		Layer prevLayer = _network[i-1];
		for(size_t k = 0; k < _network[i].size(); k++) {
			// find all connections
			std::pair<int, int> to {i,k};
			std::vector<double> weightsForNeuron;
			for(size_t j = 0; j < _network[i-1].size(); j++) {
				std::pair<int, int> from {i-1,j};
				Connection c {from, to};
				auto x = _weights.find(c);
				weightsForNeuron.push_back(x->second.weight);
			}
			_network[i][k].feedForward(prevLayer, weightsForNeuron);
			outputs.push_back(_network[i][k].getOutputValue());
		}
	}
	allNeuronOutputs = std::move(outputs);
}

void NeuralNet::backPropogation(const std::vector<double> &targetValues) {

}
		
std::vector<double> NeuralNet::getOutputs() {
	std::vector<double> outputs;
	for(size_t i = 0; i < _network[_network.size()-1].size(); i++) {
		outputs.push_back(_network[_network.size()-1][i].getOutputValue());
	}
	return outputs;
}

NeuralNet::~NeuralNet() {

}
