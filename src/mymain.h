#ifndef MYMAIN_H
#define MYMAIN_H

#include "agent.h"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/noise_texture2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <thread>

namespace godot {

class MyMain : public Node3D {
    GDCLASS(MyMain, Node3D)
    
private:
    NoiseTexture2D* texture;
    Image* image;
    double d;
    Ref<PackedScene> tree_scene;
    Ref<PackedScene> agent_scene;
    std::vector<std::thread> threads;

    void add_tree(const Vector3& spawn_loc);

protected:
    static void _bind_methods();

public:

    void set_cam_size(double size);

    int occluders;

    MyMain();
    ~MyMain();

    void _ready();
    void _process(double delta);
    
    void _on_navigation_region_3d_bake_finished();

    void give_agent_path(Agent* agent);

    void make_agent();
    void delete_agent();

    void _on_agent_count_changed(float value);
};
}



#endif