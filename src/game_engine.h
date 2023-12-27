#pragma once
#include <map>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>
#include "entity_manager.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include "assets.h"

class Scene;

typedef std::map<std::string, std::map<std::string, std::string>> Config;

class GameEngine {
    // Window init
    void create_window(
        const size_t width = 800, 
        const size_t height = 600, 
        const std::string & title = "SFML", 
        const size_t depth = 32, 
        const size_t framerate = 60, 
        const bool fullscreen = false
    );
    void create_window(
        const sf::VideoMode & mode, 
        const std::string & title, 
        const size_t framerate, 
        const bool fullscreen
    );

    // Config related
    void test_config(Config &config) const;
    Config parse_tokens(const std::vector<std::string> &tokenstream) const;
    const Config read_file(const std::string &configfile) const;
    void export_config(Config &config, const std::string &filename) const;
    const int read_config_i(const std::string &header, const std::string &value);
    const float read_config_f(
        const std::string &header,
        const std::string &value
    );
    const std::string read_config_s(
        const std::string &header,
        const std::string &value
    );
    void write_config(
        const std::string &header, 
        const std::string &item,
        const std::string &value
    );

    // Private member variables
    std::map<std::string, std::shared_ptr<Scene>> m_scenes;
    std::shared_ptr<Scene> m_currentScene;
    Assets m_assets {};
    std::string m_currentSceneName {};
    sf::Clock m_delta_clock{sf::Clock()};
    Config m_config;
    sf::RenderWindow m_window{sf::RenderWindow()};
    std::string m_userconfig_file;
    size_t m_frameCount{0};

    // State booleans
    bool m_running{true};
    bool m_paused{false};
    bool m_sGUI{true};

public:
    int m_game_close_timeout{100};
    int m_game_close_countdown{0};

    GameEngine(const std::string &userconfig, const std::string &default_config);
    ~GameEngine(){};

    void init(const std::string &configfile);
    Assets & getAssets();
    sf::RenderWindow & getWindow();
    bool changeScene(const std::string & scene_name);
    void shutdown();
    void run();
    void update();

    // Systems
    void sUserInput();
    void sDebug();
};