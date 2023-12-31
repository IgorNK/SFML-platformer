#include "scene_play.h"
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

Scene_Play::Scene_Play(GameEngine* game, const std::string & level_path)
    : Scene(game)
    , m_levelPath(level_path)
{ 
    init();
}

void Scene_Play::init() {
	std::cout << "Loading " << m_levelPath << '\n';
    
    const std::string font_path = "resources/ShareTech-Regular.ttf";
    if (!m_font.loadFromFile(font_path)) {
        std::cerr << "Failed to load font " << font_path << std::endl;
        return;
    }
    
    // m_view = sf::View();
    // m_game->getWindow().setView(m_view);
    
    if (!m_level.load_file(m_levelPath)) {
        std::cerr << "Scene failed to initialize, couldn't load " << m_levelPath << std::endl;
        return;
    }

    if (!m_tilemap.load(m_level.getTileset(), m_level.getTiles(), m_level.getDimensions().x, m_level.getDimensions().y)) {
        std::cerr << "Failed to load tilemap." << std::endl;
        return;
    }
    
    m_size_pixels = Vec2({
        m_level.getDimensions().x * m_level.getTileset().tileWidth,
        m_level.getDimensions().y * m_level.getTileset().tileHeight
    });

    m_grid_size = Vec2({
        (float)m_level.getTileset().tileWidth,
        (float)m_level.getTileset().tileHeight
    });
    
    std::cout << "Initialized level of size: " << m_size_pixels.x << ":" << m_size_pixels.y << ", grid size: " << m_grid_size.x << ":" << m_grid_size.y << std::endl;

    m_debug_grid = spawn_grid(m_size_pixels, m_grid_size);
}

void Scene_Play::onEnd() {
    m_game->getWindow().setView(m_game->getWindow().getDefaultView());
}

void Scene_Play::update() {
    sDebug();
}

void Scene_Play::sDoAction(const Action & action) {
    
}

void Scene_Play::sRender() {
    sf::RenderWindow & window = m_game->getWindow();
    if (m_sRender) {
        // m_window.draw(shape);
        window.draw(m_tilemap);
    }
    if (m_sDebugGrid) {
        for (const DebugCell & cell : m_debug_grid) {
            m_game->getWindow().draw(cell);
        }
    }
}

void Scene_Play::sDebug() {
    ImGui::Begin("Platformer Project Debug");
    ImGui::SeparatorText("Gameplay scene");
    ImGui::End();
}

std::vector<DebugCell> Scene_Play::spawn_grid(const Vec2 & size, const Vec2 & grid_size) {
    const int n_x = std::ceil(size.x / grid_size.x);
    const int n_y = std::ceil(size.y / grid_size.y);
    std::vector<DebugCell> cells;
    if (n_x <= 0 || n_y <= 0) {
        return {};
    }
    for (int i = 0; i < n_x; ++i) {
        for (int j = 0; j < n_y; ++j) {
            const DebugCell cell(grid_size, Vec2(i, j), m_font);
            cells.push_back(cell);
        }
    }
    // for (const auto & cell : cells) {
    //     std::cout << cell.getPosition().x * cell.getSize().x << ":" << cell.getPosition().y * cell.getSize().y << " - " << cell.getPosition().x * cell.getSize().x + cell.getSize().x << ":" << cell.getPosition().y * cell.getSize().y + cell.getSize().y << std::endl;
    // }
    return cells;
}
