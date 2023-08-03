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
					weights[std::pair<std::pair<int,int>, std::pair<int, int>>(from, to)] = w;
				}
			}
		}
	}

	// Neuron rendering positions
	float radius = 30;
	float neuronWidth = 70.f + radius;
	float layerWidth = 100 + radius;
	float xPos = (windowSize.x - (windowSize.x * 0.3) 
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
	// Pass inputs directly as the input layer's outputs
	for(size_t i = 0; i < _network[0].size(); i++) {
		_network[0][i].setOutputValue(inputs[i]);
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
				auto x = weights.find(c);
				weightsForNeuron.push_back(x->second.weight);
			}
			_network[i][k].feedForward(prevLayer, weightsForNeuron);
		}
	}
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
		
// visual 
// TODO move to renderer file
void NeuralNet::renderNetwork(sf::RenderWindow* window) {
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
					window->draw(line, 2, sf::Lines);
				}
			}
			window->draw(neuronShape);
		}
	}
}
