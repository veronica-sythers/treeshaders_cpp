#include "mymain.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/fast_noise_lite.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/navigation_region3d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include "oaktree.h"
#include "textures.hpp"
#include "utils.hpp"

using namespace godot;

void MyMain::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_cam_size"), &MyMain::set_cam_size);
    ClassDB::bind_method(D_METHOD("give_agent_path"), &MyMain::give_agent_path);

    ClassDB::bind_method(D_METHOD("_on_navigation_region_3d_bake_finished"), &MyMain::_on_navigation_region_3d_bake_finished);
    ClassDB::bind_method(D_METHOD("_on_agent_count_changed"), &MyMain::_on_agent_count_changed);
}

MyMain::MyMain() {

}

MyMain::~MyMain() {

}

void MyMain::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    MyTextures::getInstance();
    d = 0;
    occluders = 0;

    tree_scene = ResourceLoader::get_singleton()->load("res://tree.tscn");
    agent_scene = ResourceLoader::get_singleton()->load("res://agent.tscn");

    texture = new NoiseTexture2D();
    texture->set_noise(new FastNoiseLite());
    image = new Image();
    image = *texture->get_image();

    int max = 100;
    for (int x = 0; x != max; x++) { 
        for (int y = 0; y != max; y++) {
            double xx = remap(x, 0, max, 0, 512);
            double yy = remap(y, 0, max, 0, 512);
            if (texture->get_noise()->get_noise_2d(xx, yy) > 0.1) {
                double r = 0.25;
                Vector3 spawn_loc = Vector3(
                    remap(x, 0, max, -36, 36),
                    0,
                    remap(y, 0, max, -36, 36)
                );
                add_tree(spawn_loc);
            }
        }
    }

    get_node<NavigationRegion3D>("NavigationRegion3D")->bake_navigation_mesh();
}

void MyMain::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    d += delta;
    if (d > 2) {

    }

    get_node<Label>("Control/fps")->set_text(
        String("FPS: %s") % Engine::get_singleton()->get_frames_per_second()
    );
    get_node<Label>("Control/trees")->set_text(
        String("Trees: %s") % (int64_t)(get_node<Node3D>("NavigationRegion3D/trees")->get_child_count())
    );
    get_node<Label>("Control/occluders")->set_text(
        String("Occluders: %s") % (int64_t)(occluders)
    );
    occluders = 0;
}

void MyMain::add_tree(const Vector3& spawn_loc) {
    OakTree* tree = (OakTree*)(tree_scene->instantiate());
    tree->set_position(spawn_loc);
    float rot_y = static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 720 - 360;
    tree->set_rotation(Vector3(tree->get_rotation().x, rot_y, tree->get_rotation().z));
    get_node<Node3D>("NavigationRegion3D/trees")->add_child(tree);
}

void MyMain::set_cam_size(double size) {
    get_node<Label>("Control/cam_size")->set_text(
        String("CAM SIZE: %s") % size
    );
}

void MyMain::_on_navigation_region_3d_bake_finished() {
    for (int i = 0; i != 2000; i++) {
        make_agent();
    }
}

void MyMain::make_agent() {
    Agent* agent = (Agent*)(agent_scene->instantiate());
    agent->set_position(Vector3(
        static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 40 - 20,
        0.1,
        static_cast<float>(rand()) / static_cast <float> (RAND_MAX) * 40 - 20
    ));
    agent->set_activated(true);
    Callable callable(this, "give_agent_path");
    Array args;
    args.append(agent);
    callable.bindv(args);
    agent->connect("path", callable);
    agent->init(get_node<Node3D>("agents")->get_child_count());
    get_node<Node3D>("agents")->add_child(agent);
}

void MyMain::give_agent_path(Agent* agent) {
    Array targets = get_node<Node3D>("NavigationRegion3D/trees")->get_children();
    Variant target = targets[rand() % targets.size()]; 
    Node3D* t2 = godot::Object::cast_to<godot::Node3D>(target);
    agent->goto_target(t2);
}

void MyMain::delete_agent() {
    Node* a = get_node<Node>("agents")->get_child(
        get_node<Node3D>("agents")->get_child_count() - 1);
    std::cout << ((Node3D*)(a))->get_position().x << std::endl;
    a->queue_free();
}

void MyMain::_on_agent_count_changed(float value) {
    int v = (int)value - get_node<Node3D>("agents")->get_child_count();
    std::cout << "AGENTS: " << get_node<Node3D>("agents")->get_child_count() << std::endl;
    std::cout << "V: " << v << std::endl;
    if (v > 0) {
        for (int i = 0; i != v; i++) {
            make_agent();
        }
    } else {
        for (int i = 0; i != abs(v); i++) {
            delete_agent();
        }
    }
}