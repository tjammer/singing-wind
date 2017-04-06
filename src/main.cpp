#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include "Engine.h"

int main() {

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Editor", sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);
    Engine engine(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::GainedFocus) {
                engine.set_focus(true);
            }
            else if (event.type == sf::Event::LostFocus) {
                engine.set_focus(false);
            }
        }
        engine.update();

        window.display();
    }

    return 0;
}