#include "SFML/Graphics.hpp"
#include "Engine.h"
#include <imgui-SFML.h>
#include <imgui.h>

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
    ImGui::SFML::Init(window);

    sf::Clock imgui_clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::GainedFocus) {
                engine.set_focus(true);
            }
            else if (event.type == sf::Event::LostFocus) {
                engine.set_focus(false);
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    engine.update_mouse_wheel(event.mouseWheelScroll.delta);
                }
            }
        }
        ImGui::SFML::Update(window, imgui_clock.restart());
        engine.update();

        ImGui::Render();
        window.display();
    }
    return 0;
}