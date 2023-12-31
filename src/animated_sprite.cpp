#include "animated_sprite.h"
#include <fstream>
#include <iostream>
#include <sstream>

void AnimatedSprite::update() {
    
}

bool AnimatedSprite::hasEnded() {
    std::vector<sf::IntRect> & frames = m_animations[m_currentAnimation];
    return m_currentFrame >= frames.size() - 1;
}

bool AnimatedSprite::load_file(const std::string & path) {
    std::cout << "Loading atlas data: " << path << std::endl;
    std::fstream atlas_file;
    atlas_file.open(path, std::ios::in);
    if (!atlas_file.is_open()) {
        throw std::runtime_error("Could not read file " + path);
        return false;
    }

    std::string word, imagePath, size, repeat;
    std::getline(atlas_file, imagePath);
    std::getline(atlas_file, size);
    std::getline(atlas_file, repeat);

    if (imagePath == "") {
        throw std::runtime_error("Atlas file " + path + " does not contain image path.");
        return false;
    }

    std::string::size_type path_delim = path.find_last_of("\\/");
    std::string path_dir = path.substr(0, path_delim);
    std::cout << "loading image " << path_dir + '/' + imagePath << std::endl;;

    sf::Texture texture;
    if (!texture.loadFromFile(path_dir + '/' + imagePath)) {
        throw std::runtime_error("Couldn't load texture " + imagePath);
        return false;
    }

    m_sprite.setTexture(texture);

    std::string::size_type size_s = size.find(':');
    std::string::size_type size_d = size.find(',', size_s);
    // std::cout << "image width: " << size.substr(size_s + 1, size_d - size_s - 1) << std::endl;
    // std::cout << "image height: " << size.substr(size_d + 1, size.size()) << std::endl;
    const int image_width = std::stoi(size.substr(size_s + 1, size_d - size_s - 1));
    const int image_height = std::stoi(size.substr(size_d + 1, size.size()));
    m_imageSize.x = image_width;
    m_imageSize.y = image_height;

    std::string::size_type repeat_s = repeat.find(':', 0);
    m_repeat = repeat.substr(repeat_s, repeat.size() - repeat_s);
    
    std::string line = "";
    std::string heading = "";
    int index = 0;
    while (std::getline(atlas_file, line)) {

        if (line.find(':') == std::string::npos) {
            heading = line;
            // std::cout << "heading: " << heading << std::endl;
            if (m_animations.find(heading) == m_animations.end()) {
                // std::cout << "This is a new heading, adding an array.\n";
                m_animations[heading] = {};
                m_animations[heading].resize(1);
                index = 0;
            }
            continue;
        }

        std::string key, value;
        std::string::size_type begin = line.find_first_not_of(" ,[\f\t\v");
        
        // Extract key value
        std::string::size_type end = line.find(':', begin);
        key = line.substr(begin, end - begin);
        if (key.empty()) {
            continue;
        }

        // Extract value
        begin = line.find_first_not_of(" ,]\f\n\r\t\v", end + 1);
        end = line.find_last_not_of(" ,]\f\n\r\t\v") + 1;
        value = line.substr(begin, end - begin);

        if (key == "index") {
            //Need to check the size of IntRect vector, and resize if needed
            //But first, if we don't have a header yet, something went wrong:
            if (m_animations.find(heading) == m_animations.end()) {
                throw std::runtime_error("Header " + heading + " wasn't yet written");
                return false;
            }
            index = std::stoi(value);
            if (m_animations[heading].size() < index + 1) {
                // std::cout << "This index is more than array capacity, resizing from " << m_animations[heading].size() << " to " << index + 1 << ".\n";
                m_animations[heading].resize(index + 1);
            }
        }

        if (key == "bounds") {
            std::string s_x, s_y, s_w, s_h;
            int x, y, w, h;
            std::istringstream values;
            values.str(value);
            std::getline(values, s_x, ',');
            std::getline(values, s_y, ',');
            std::getline(values, s_w, ',');
            std::getline(values, s_h, ',');
            x = std::stoi(s_x);
            y = std::stoi(s_y);
            w = std::stoi(s_w);
            h = std::stoi(s_h);
            m_animations[heading][index] = sf::IntRect({x, y, w, h});
            // std::cout << "Recorded bounds: " << s_x << ":" << s_y << "," << s_w << "," << s_h << std::endl;
        }
    }
    return true;
}

void AnimatedSprite::test_data() {
    std::cout << "Starting animated sprite data test:\n";
    std::cout << "Image size: " << m_imageSize.x << "," << m_imageSize.y << std::endl;
    std::cout << "Repeat: " << m_repeat << std::endl;
    std::for_each(
        m_animations.begin(), m_animations.end(),
        [&](std::pair<std::string, std::vector<sf::IntRect>> v)
        {
            std::cout << v.first << ":\n";
            for (const sf::IntRect & rect : v.second) {
                std::cout << rect.left << ":" << rect.top << "," << rect.width << ":" << rect.height << std::endl;
            }
        }
    );
}

const Vec2 & AnimatedSprite::getSize() const {
    return m_size;
}

sf::Sprite & AnimatedSprite::getSprite() {
    return m_sprite;
}