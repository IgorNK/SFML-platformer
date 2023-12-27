#include "../game_engine.h"
#include <SFML/Window/Event.hpp>
#include <iostream>

void GameEngine::sUserInput() {
    sf::Event event;

    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::F5: {
                    m_sGUI = !m_sGUI;
                    break;
                }
                case sf::Keyboard::F1: {
                    m_paused = !m_paused;
                    break;
                }
                case sf::Keyboard::Escape: {
                    if (m_game_close_countdown > 0) {
                        m_running = false; 
                    } else {
                        m_game_close_countdown = m_game_close_timeout;
                    }
                    break;
                }
            }
        }

        if (event.type == sf::Event::MouseMoved) {
            const sf::Vector2i mouse_pos = sf::Mouse::getPosition(m_window);
        }
    }
}