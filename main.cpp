#include "NeuralNet.h"
#include "include/EventManager.hpp"


int main(int argc, char *argv[]) {

	srand(static_cast<unsigned>(time(0)));

	std::vector<int> topology;
	topology.push_back(2);
	topology.push_back(3);
	topology.push_back(3);

	sf::Vector2u windowSize {800, 600};
	sf::RenderWindow* window = 
		new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y), "Neural Network");
	window->setPosition(sf::Vector2i(900, 100));
	const float panelX = windowSize.x - (windowSize.x * 0.3);

	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(panelX, 0), sf::Color::White),
		sf::Vertex(sf::Vector2f(panelX, windowSize.y), sf::Color::White)
	};
	
	NeuralNet neural_network { NeuralNet(topology, windowSize) };

	// event handling & key bindings
	sfml_evm::EventManager evm(*window, true);
	evm.addEventCallback(sf::Event::EventType::Closed, 
		[&](const sf::Event &){ window->close(); });
	evm.addKeyPressedCallback(sf::Keyboard::Escape, 
		[&](const sf::Event &){ window->close(); });
	// Forward Propogation 
	std::vector<double> testInput;
	testInput.push_back(5);
	testInput.push_back(3);
	evm.addKeyPressedCallback(sf::Keyboard::F,
		[&](const sf::Event &){ 
		std::cout << "Forward Propogation" << std::endl; 
		neural_network.feedForward(testInput);
		std::cout << "results" << std::endl;
		std::vector<double> results = std::move(neural_network.getOutputs());
		for(double r : results) {
			std::cout << r << std::endl;
		}
	});

	while(window->isOpen()) {

		evm.processEvents();
		window->clear();
		neural_network.renderNetwork(window);

		// render panel
		window->draw(line, 3, sf::Lines);

		window->display();
	}

	delete window;

	return 0;
}
