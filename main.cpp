#include "NeuralNet.h"
#include <fstream>
#include "include/EventManager.hpp"
#include "include/Renderer.hpp"

/*
 * Program key bindings
 * ESC - close program
 * S - save network parameters to file
 * L - load network parameters from file
 * T - train network on 100000 iterations
 * R - run network 
*/


int main(int argc, char *argv[]) {

	srand(static_cast<unsigned>(time(0)));

	// network topology
	std::vector<int> topology;
	topology.push_back(2);
	topology.push_back(2);
	topology.push_back(1);

	// window
	sf::Vector2u windowSize {800, 600};
	sf::RenderWindow* window = 
		new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y), "Neural Network");
	window->setPosition(sf::Vector2i(900, 100));
	
	// TODO Panel line consider new window instead
	NeuralNet neural_network { NeuralNet(topology, windowSize) };
	
	// rendering
	sfml_ren::Renderer render(*window);

	// file 
	std::ofstream outfile;
	std::ifstream infile;

	// event handling & key bindings
	sfml_evm::EventManager evm(*window, true);
	evm.addEventCallback(sf::Event::EventType::Closed, 
		[&](const sf::Event &){ window->close(); });
	evm.addKeyPressedCallback(sf::Keyboard::Escape, 
		[&](const sf::Event &){ window->close(); });
	// neural network saving and loading
	evm.addKeyPressedCallback(sf::Keyboard::S,
		[&](const sf::Event &){
			neural_network.getNetworkBiases();
			neural_network.getNetworkWeights();
			std::cout << "save network parameters" << std::endl;
			outfile.open("weights.txt");
			outfile << neural_network;
			outfile.close();
		});
	evm.addKeyPressedCallback(sf::Keyboard::L,
		[&](const sf::Event &){
			std::cout << "load network parameters" << std::endl;
			infile.open("weights.txt");
			infile >> neural_network;
			infile.close();
		});

	// Forward Propogation 
	// Example for training XOR problem
	typedef std::vector<std::vector<double>> trainingData;
	trainingData input;
	std::vector<double> testInput1;
	testInput1.push_back(1);
	testInput1.push_back(0);
	std::vector<double> testInput2;
	testInput2.push_back(1);
	testInput2.push_back(1);
	std::vector<double> testInput3;
	testInput3.push_back(0);
	testInput3.push_back(1);
	std::vector<double> testInput4;
	testInput4.push_back(0);
	testInput4.push_back(0);
	input.push_back(testInput1);
	input.push_back(testInput2);
	input.push_back(testInput3);
	input.push_back(testInput4);

	trainingData output;
	std::vector<double> testOutput1;
	testOutput1.push_back(1.0);
	std::vector<double> testOutput2;
	testOutput2.push_back(0.0);
	std::vector<double> testOutput3;
	testOutput3.push_back(1.0);
	std::vector<double> testOutput4;
	testOutput4.push_back(0.0);
	output.push_back(testOutput1);
	output.push_back(testOutput2);
	output.push_back(testOutput3);
	output.push_back(testOutput4);

	// training function
	auto train = [&](){
		bool printInfo = false;
		
		int index = rand() % 4;
		std::vector<double> testOutput = output[index];
		std::vector<double> testInput = input[index];
		
		if(printInfo) {
			std::cout << "-----------Forward Propogation----------" << std::endl; 
			std::cout << "---Inputs---" << std::endl;
			for(double x : testInput) {
				std::cout << x << std::endl;
			}
				
			std::cout << "---Weights---" << std::endl;
			std::vector<double> cpyW = std::move(neural_network.getNetworkWeights());
			for(double x : cpyW){
				std::cout << x << std::endl;
			}
			
			std::cout << "---Biases---" << std::endl;
			std::vector<double> cpyB = std::move(neural_network.getNetworkBiases());
			for(double x : cpyB){
				std::cout << x << std::endl;
			}
			
			std::cout << "---Results---" << std::endl;
			std::vector<double> results = std::move(neural_network.getOutputs());
			for(double r : results) {
				std::cout << r << std::endl;
			}
		}

		neural_network.feedForward(testInput);
		neural_network.backPropogation(testOutput);
	};

	// training key bindings
	int trainingIteration = 1000000;
	evm.addKeyPressedCallback(sf::Keyboard::T,
		[&](const sf::Event &){ 
		trainingIteration = 0;
	});
	evm.addKeyPressedCallback(sf::Keyboard::R,
		[&](const sf::Event &){ 
		train();
	});

	while(window->isOpen()) {

		evm.processEvents();
		window->clear();
		
		// rendering the network topology
		render.renderNetwork(neural_network.getNetworkPositions(), 
			neural_network.getNetworkWeights(),
			neural_network.getAllOutputs());

		// Check if training
		if(trainingIteration < 100000) {
			train();
			trainingIteration ++;
		}

		window->display();
	}

	// when the program ends
	delete window;

	return 0;
}
