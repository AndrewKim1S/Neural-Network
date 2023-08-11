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
	_numNeurons = neuronId;
	_networkBiases = std::vector<double>(_numNeurons, 0.0);

	// Randomize Weights
	for(size_t i = 0; i < _network.size(); i++) {
		for(size_t k = 0; k < _network[i].size(); k++) {
			if(i+1 != _network.size()) {
				for(size_t j = 0; j < _network[i+1].size(); j++) {
					std::pair<int, int> from {i, k};
					std::pair<int, int> to {i+1, j};
					Weight w;
					w.weight = ((double)rand()) / RAND_MAX;
					_networkWeights.push_back(w.weight);
					_weights[std::pair<std::pair<int,int>, std::pair<int, int>>(from, to)] = w;
				}
			}
		}
	}

	// Neuron rendering positions
	// TODO Remove SFML dependencies
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
	// calculate cost 
	double cost = 0;
	for(size_t i = 0; i < targetValues.size(); i++) {
		double error = targetValues[i] - _network[_network.size()-1][i].getOutputValue();
		cost += error * error;
	}

	// output layer gradient calculation 
	Layer &outputLayer = _network.back();
	for(size_t n = 0; n < outputLayer.size(); n++) {
		outputLayer[n].calculateOutputGradient(targetValues[n]);
	}

	// calculate the sum of the errors we feed 
	auto sumErrorsFed = [this](Layer &nextLayer, std::pair<int, int> currentNeuron){
		double sum = 0.0;
		std::pair<int, int> connecting;
		connecting.first = currentNeuron.first +1;
		for(size_t n = 0; n < nextLayer.size(); n++) {
			connecting.second = n;
			
			auto w = _weights.find(Connection(currentNeuron, connecting));
			
			sum += w->second.weight * nextLayer[n].getGradient();
		}
		return sum;
	};

	// hidden layers gradient calculation
	for(size_t layer = _network.size() -2; layer > 0; layer--) {
		Layer &hiddenLayer = _network[layer];
		Layer &nextLayer = _network[layer+1];
		for(size_t n = 0; n < hiddenLayer.size(); n++) {
			std::pair<int, int> currentNeuron {layer, n};
			hiddenLayer[n].calculateHiddenGradient(sumErrorsFed(nextLayer, currentNeuron));
		}
	}

	auto updateConnections = [this](Layer &prevLayer, std::pair<int, int> current) {
		std::pair<int, int> connecting;
		connecting.first = current.first-1;
		for(size_t n = 0; n < prevLayer.size(); n++) {
			connecting.second = n;
			auto w = _weights.find(Connection(connecting, current));
			w->second.weight += _learningRate * 
				_network[current.first][current.second].getGradient() * 
				_network[connecting.first][connecting.second].getOutputValue();
			_network[current.first][current.second].
				changeBias(_learningRate * _network[current.first][current.second].getGradient());
		}
	};

	// Update weights & biases appropriately
	for(size_t layer = _network.size() - 1; layer > 0; layer--) {
		Layer &prevLayer = _network[layer-1];
		for(size_t n = 0; n < _network[layer].size(); n++) {
			std::pair<int, int> currentNeuron {layer, n};
			updateConnections(prevLayer, currentNeuron);
		}
	}
}
		
std::vector<double> NeuralNet::getOutputs() {
	std::vector<double> outputs;
	for(size_t i = 0; i < _network[_network.size()-1].size(); i++) {
		outputs.push_back(_network[_network.size()-1][i].getOutputValue());
	}
	return outputs;
}

std::istream &operator>>(std::istream &is, NeuralNet &network) {
		
	return is;
}

std::ostream& operator<<(std::ostream &out, const NeuralNet &network) {
	out << "Weights\n";
	for(auto it = network._weights.begin(); it != network._weights.end(); it++) {
		out << it->first.first.first << "," << it->first.first.second << " " <<
			it->first.second.first << "," << it->first.second.second << " " << 
			it->second.weight << "\n";
	}
	out << "Biases\n";
	for(auto it = network._networkBiases.begin(); 
		it != network._networkBiases.end(); it++) {
		out << *it << "\n";
	}
	return out;
}

NeuralNet::~NeuralNet() {

}

