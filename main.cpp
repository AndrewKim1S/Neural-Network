#include "NeuralNet.h"
#include "include/EventManager.hpp"
#include "include/Renderer.hpp"


int main(int argc, char *argv[]) {

	srand(static_cast<unsigned>(time(0)));

	std::vector<int> topology;
	topology.push_back(2);
	topology.push_back(2);
	topology.push_back(1);

	sf::Vector2u windowSize {800, 600};
	sf::RenderWindow* window = 
		new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y), "Neural Network");
	window->setPosition(sf::Vector2i(900, 100));
	

	// TODO Panel line consider new window instead
	//const float panelX = windowSize.x - (windowSize.x * 0.3);
	/*
	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(panelX, 0), sf::Color::White),
		sf::Vertex(sf::Vector2f(panelX, windowSize.y), sf::Color::White)
	};*/
	
	NeuralNet neural_network { NeuralNet(topology, windowSize) };

	// event handling & key bindings
	sfml_evm::EventManager evm(*window, true);
	evm.addEventCallback(sf::Event::EventType::Closed, 
		[&](const sf::Event &){ window->close(); });
	evm.addKeyPressedCallback(sf::Keyboard::Escape, 
		[&](const sf::Event &){ window->close(); });

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
	
	auto train = [&](){
		std::cout << "-----------Forward Propogation----------" << std::endl; 
		int index = rand() % 4;
		std::vector<double> testOutput = output[index];
		std::vector<double> testInput = input[index];
		
		std::cout << "---Inputs---" << std::endl;
		for(double x : testInput) {
			std::cout << x << std::endl;
		}
		std::cout << "---Weights---" << std::endl;
		std::vector<double> cpy = std::move(neural_network.getNetworkWeights());
		for(double x : cpy){
			std::cout << x << std::endl;
		}
		neural_network.feedForward(testInput);
		std::cout << "---Results---" << std::endl;
		std::vector<double> results = std::move(neural_network.getOutputs());
		for(double r : results) {
			std::cout << r << std::endl;
		}
		neural_network.backPropogation(testOutput);
	};

	evm.addKeyPressedCallback(sf::Keyboard::F,
		[&](const sf::Event &){ 
		train();
	});

	
	// rendering
	sfml_ren::Renderer render(*window);

	int iteration = 0;
	while(window->isOpen()) {

		evm.processEvents();
		window->clear();
		
		// rendering the network topology
		// neural_network.renderNetwork(window);  
		render.renderNetwork(neural_network.getNetworkPositions(), 
			neural_network.getNetworkWeights(),
			neural_network.getAllOutputs());

		if(iteration < 500000) {
			train();
			iteration ++;
		}

		window->display();
	}

	delete window;

	return 0;
}
