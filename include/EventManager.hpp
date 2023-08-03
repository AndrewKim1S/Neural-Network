#pragma once

/* Taken from John Buffer
 * https://github.com/johnBuffer/SFML-EventManager/blob/master/inlcude/event_manager.hpp
*/

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include <functional>
#include <iostream> //remove

namespace sfml_evm {

// function type
using EventCallback = std::function<void(const sf::Event& event)>;

// map of callback functions
template<class T>
using EventCallbackMap = std::unordered_map<T, EventCallback>;
using CstEv = const sf::Event&;


// Class handles subtyped events such as keyboard or mouse events
// unpack function will get relevant information from the event
template<class T>
class SubTypeManager {
	
	public:
		SubTypeManager(std::function<T(const sf::Event&)> unpack) :
			_unpack(unpack) {}
		~SubTypeManager() = default;

		void processEvent(const sf::Event& event) const {
			T sub_value = _unpack(event);
			// call associated callback from event
			auto it(_callmap.find(sub_value));
			if(it != _callmap.end()) {
				(it->second)(event);
			}
		}

		// add subtypevalue and callback into map as key value pair
		void addCallback(const T& sub_value, EventCallback callback) {
			_callmap[sub_value] = callback;
		}

	private:
		EventCallbackMap<T> _callmap;
		std::function<T(const sf::Event&)> _unpack;
};


class EventMap {
	
	public:
		EventMap(bool use_builtin_helpers = true) :
			// In the initializer list, we set SubTypeManagers with lamda expressions
			_key_pressed_manager([](const sf::Event &event) { return event.key.code; }),
			_key_released_manager([](const sf::Event &event) { return event.key.code; }),
			_mouse_pressed_manager([](const sf::Event &event) 
				{ return event.mouseButton.button; }),
			_mouse_released_manager([](const sf::Event &event) 
				{ return event.mouseButton.button; }) {
			if(use_builtin_helpers) {
				// Register key events built in callbacks
				this->addEventCallback(sf::Event::EventType::KeyPressed, 
					[&](const sf::Event &event) { _key_pressed_manager.processEvent(event); });
				this->addEventCallback(sf::Event::EventType::KeyReleased,
					[&](const sf::Event &event) {_key_released_manager.processEvent(event); });
				this->addEventCallback(sf::Event::EventType::MouseButtonPressed,
					[&](const sf::Event &event) {_mouse_pressed_manager.processEvent(event); });
				this->addEventCallback(sf::Event::EventType::MouseButtonReleased,
					[&](const sf::Event &event) {_mouse_released_manager.processEvent(event); });
			}
		}

		// attaches new callback to an event
		void addEventCallback(sf::Event::EventType type, EventCallback callback) {
			_events_callmap[type] = callback;
		}

		// adds key pressed callback
		void addKeyPressedCallback(sf::Keyboard::Key key_code, EventCallback callback) {
			_key_pressed_manager.addCallback(key_code, callback);
		}

		// adds key released callback
		void addKeyReleasedCallback(sf::Keyboard::Key key_code, EventCallback callback) {
			_key_released_manager.addCallback(key_code, callback);
		}

		// add mouse pressed callback
		void addMousePressedCallback(sf::Mouse::Button button, EventCallback callback) {
			_mouse_pressed_manager.addCallback(button, callback);
		}

		// add mouse released callback
		void addMouseReleasedCallback(sf::Mouse::Button button, EventCallback callback) {
			_mouse_released_manager.addCallback(button, callback);
		}

		// Runs the callback associated with an event
		void executeCallback(const sf::Event &e, EventCallback fallback = nullptr) const {
			auto it(_events_callmap.find(e.type));
			if(it != _events_callmap.end()) {
				(it->second)(e);
			}
			else if (fallback) {
				fallback(e);
			}
		}

		// remove a callback
		void removeCallback(sf::Event::EventType type) {
			auto it(_events_callmap.find(type));
			if(it != _events_callmap.end()) {
				// remove associated callback
				_events_callmap.erase(it);
			}
		}

	private:
		SubTypeManager<sf::Keyboard::Key> _key_pressed_manager;
		SubTypeManager<sf::Keyboard::Key> _key_released_manager;
		SubTypeManager<sf::Mouse::Button> _mouse_pressed_manager;
		SubTypeManager<sf::Mouse::Button> _mouse_released_manager;
		// unordered map of callbacks with event type key
		EventCallbackMap<sf::Event::EventType> _events_callmap;
};


/*
 * This class handles any type of event and call its associated callbacks if any
 */
class EventManager {

	public:
		// EventManager() {};
		EventManager(sf::Window &window, bool use_builtin_helpers) :
			_window(window), _event_map(use_builtin_helpers) {}
		/*
		// Copy constructor
		EventManager( const EventManager &other) {
			_window = other._window;
			_event_map = other._event_map;
		}*/




		// call events attachted callbacks
		void processEvents(EventCallback fallback = nullptr) const {
			// iterate over events
			sf::Event event;
			while(_window.pollEvent(event)) {
				_event_map.executeCallback(event, fallback);
			}
		}

		// attaches new callback to an event
		void addEventCallback(sf::Event::EventType type, EventCallback callback) {
			_event_map.addEventCallback(type, callback);
		}

		// remove callback
		void removeCallback(sf::Event::EventType type) {
			_event_map.removeCallback(type);
		}

		// add key pressed callback
		void addKeyPressedCallback(sf::Keyboard::Key key, EventCallback callback) {
			_event_map.addKeyPressedCallback(key, callback);
		}
		
		// add key released callback
		void addKeyReleasedCallback(sf::Keyboard::Key key, EventCallback callback) {
			_event_map.addKeyReleasedCallback(key, callback);
		}

		// add mouse pressed callback
		void addMousePressedCallback(sf::Mouse::Button button, EventCallback callback) {
			_event_map.addMousePressedCallback(button, callback);
		}
		
		// add mouse released callback
		void addMouseReleasedCallback(sf::Mouse::Button button, EventCallback callback) {
			_event_map.addMouseReleasedCallback(button, callback);
		}

		sf::Window& getWindow() {
			return _window;
		}

		sf::Vector2f getFloatMousePosition() const {
			const sf::Vector2i mouse_position = sf::Mouse::getPosition(_window);
			return {static_cast<float>(mouse_position.x), 
				static_cast<float>(mouse_position.y)};
		}

		sf::Vector2i getMousePosition() const {
			return sf::Mouse::getPosition(_window);
		}

	private:
		sf::Window& _window;
		EventMap _event_map;
};

}
