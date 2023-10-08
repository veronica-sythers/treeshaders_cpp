#ifndef TEXTURES_H
#define TEXTURES_H

#include <godot_cpp/classes/image_texture.hpp>
#include <string>


class MyTextures {
private:
    MyTextures();

    static MyTextures* instance;
public:
    MyTextures(MyTextures &other) = delete;
    void operator=(const MyTextures &) = delete;
    static MyTextures* getInstance() {
        if (instance == nullptr) {
            instance = new MyTextures();
        }
        return instance;
    }
    static std::unordered_map<std::string, godot::ImageTexture*> my_textures;

    godot::ImageTexture* get_texture(const std::string& path);
};


#endif