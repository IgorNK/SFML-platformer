#include "game_engine.h"
#include "scenes/menu/scene_menu.h"
#include "scenes/play/scene_play.h"
#include <iostream>

void GameEngine::run() {

  while (m_running) {
    // create/cleanup entities

    sf::Time deltaTime = m_delta_clock.restart();

    ImGui::SFML::Update(m_window, deltaTime);

    sUserInput();
    update();

    if (m_game_close_countdown > 0) {
      --m_game_close_countdown;
    }
  }
  shutdown();
}

void GameEngine::update() {
  m_window.clear();
  if (m_currentScene) {
    m_currentScene->update();
    m_currentScene->sRender();
  }
  sDebug();
  ImGui::SFML::Render(m_window);
  m_window.display();
}

GameEngine::GameEngine(const std::string &userconfig,
                       const std::string &default_config) {
  m_userconfig_file = userconfig;
  try {
    init(userconfig);
  } catch (std::exception &e) {
    try {
      init(default_config);
    } catch (std::exception &e) {
      throw std::runtime_error(std::string(e.what(), sizeof(e.what())) +
                               " | Could not initialize.");
    }
  }
}

void GameEngine::init(const std::string &configfile) {
  try {
    m_config = read_file(configfile);
  } catch (std::exception &e) {
    throw std::runtime_error(
        std::string(e.what(), sizeof(e.what())) +
        " | Could not read configuration file: " + configfile);
  }
  m_game_close_timeout = read_config_i("Global", "gameCloseTimeout");

  const std::string assets_file = "resources/assets.txt";
  try {
    m_assets.load_assets(assets_file);
  } catch (std::exception &e) {
    throw std::runtime_error(
        std::string(e.what(), sizeof(e.what())) +
        " | Could not read configuration file: " + assets_file);
  }
  // const std::string font_path = read_config_s("Font", "path");
  // m_font = sf::Font();
  // if (!m_font.loadFromFile(font_path)) {
  // 	throw std::runtime_error("Could load font at: " + font_path);
  // }

  std::srand(std::time(nullptr));
  const size_t width = read_config_i("Window", "width");
  const size_t height = read_config_i("Window", "height");
  const size_t depth = read_config_i("Window", "depth");
  const size_t framerate = read_config_i("Window", "refreshRate");
  const bool fullscreen = read_config_i("Window", "fullscreen");

  create_window(width, height, "ImGUI + SFML = <3", depth, framerate,
                fullscreen);

  m_scenes["MENU"] = std::make_shared<Scene_Menu>(this);
  // m_scenes["PLAY"] = std::make_shared<Scene_Play>(this, "");
  changeScene("MENU");
}

void GameEngine::create_window(const size_t width, const size_t height,
                               const std::string &title, const size_t depth,
                               const size_t framerate, const bool fullscreen) {
  if (fullscreen) {
    sf::VideoMode mode = sf::VideoMode::getDesktopMode();
    m_window.create(mode, title, sf::Style::Fullscreen);
    std::cout << "created window: " << mode.width << "x" << mode.height << " ("
              << mode.bitsPerPixel << ")\n";
  } else {
    m_window.create(sf::VideoMode(width, height, depth), title);
    std::cout << "created window: " << width << "x" << height << " (" << depth
              << ")\n";
  }
  m_window.setFramerateLimit(framerate);
  std::cout << "set framerate limit: " << framerate << '\n';
  ImGui::SFML::Init(m_window);
  std::cout << "initialized imgui\n";
}

void GameEngine::create_window(const sf::VideoMode &mode,
                               const std::string &title, const size_t framerate,
                               const bool fullscreen) {
  create_window(mode.width, mode.height, title, mode.bitsPerPixel, framerate,
                fullscreen);
}

void GameEngine::shutdown() {
  m_window.close();
  ImGui::SFML::Shutdown();
}

bool GameEngine::changeScene(const std::string &scene_name,
                             const std::shared_ptr<Scene> scene) {
  m_scenes[scene_name] = scene;
  m_currentSceneName = scene_name;
  m_currentScene = m_scenes[scene_name];
  return true;
}

bool GameEngine::changeScene(const std::string &scene_name) {
  if (m_scenes.find(scene_name) == m_scenes.end()) {
    std::cerr << "No scene named " << scene_name << std::endl;
    return false;
  }
  m_currentSceneName = scene_name;
  m_currentScene = m_scenes[scene_name];
  return true;
}

sf::RenderWindow &GameEngine::getWindow() { return m_window; }

void GameEngine::sDebug() {
  if (m_game_close_countdown > 0) {
    ImGui::Begin("Quit game");
    ImGui::Text("Press [ESC] again to close game");
    ImGui::ProgressBar((float)m_game_close_countdown /
                       (float)m_game_close_timeout);
    ImGui::End();
  }
}
