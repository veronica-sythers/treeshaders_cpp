#include "textures.hpp"

#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;


MyTextures* MyTextures::instance = nullptr;
std::unordered_map<std::string, godot::ImageTexture*> MyTextures::my_textures = {};


MyTextures::MyTextures() {
    my_textures.emplace("1a", (ImageTexture*)(*ResourceLoader::get_singleton()->load("res://1a.png")));
    my_textures.emplace("1b", (ImageTexture*)(*ResourceLoader::get_singleton()->load("res://1b.png")));
    my_textures.emplace("1c", (ImageTexture*)(*ResourceLoader::get_singleton()->load("res://1c.png")));
}

ImageTexture* MyTextures::get_texture(const std::string& path) {
    return my_textures[path];
}