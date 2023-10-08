#include "oaktree.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include "textures.hpp"
#include "utils.hpp"


using namespace godot;

void OakTree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_area_3d_area_entered"), &OakTree::_on_area_3d_area_entered);
    ClassDB::bind_method(D_METHOD("_on_area_3d_area_exited"), &OakTree::_on_area_3d_area_exited);

    ClassDB::bind_method(D_METHOD("set_wind_strength", "value", "min_value", "max_value"), &OakTree::set_wind_strength);
    ClassDB::bind_method(D_METHOD("set_wind_rotation", "rads"), &OakTree::set_wind_rotation);
}

double OakTree::get_random_double(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd()); // Mersenne twister random number generator
    static std::uniform_real_distribution<float> dist(min, max);

    return dist(gen);
}

OakTree::OakTree() {
    // Initialize any variables here.
}

OakTree::~OakTree() {
    // Add your cleanup here.
}

void OakTree::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    myroot = (MyMain*)(get_parent()->get_parent()->get_parent());
    last_center_size = 0;

    collision = get_node<CollisionShape3D>("Area3D/CollisionShape3D");
    
    l1 = get_node<MeshInstance3D>("leaves_1");
    l2 = get_node<MeshInstance3D>("leaves_2");
    l3 = get_node<MeshInstance3D>("leaves_3");

    l1->set_material_override(l1->get_material_override()->duplicate());
    l2->set_material_override(l2->get_material_override()->duplicate());
    l3->set_material_override(l3->get_material_override()->duplicate());
    // set_high_lod();

    set_graphics("dynamic");

    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter("rotation", 90 - get_rotation().y);
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter("rotation", 90 - get_rotation().y);
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter("rotation", 90 - get_rotation().y);

    double to = get_random_double(0.8, 1.2);
    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter("time_offset", to);
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter("time_offset", to);
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter("time_offset", to);

    Array dv = dummy_vignettes();
    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter("vignette_centers", dv);
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter("vignette_centers", dv);
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter("vignette_centers", dv);

    add_to_group("trees");
}

void OakTree::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    centers.clear();
    for (Area3D* a : occluders) {
        Vector3 gp = get_global_position() - a->get_global_position();
        Vector3 s = ((BoxShape3D*)(*collision->get_shape()))->get_size();
        Vector2 gp2 = Vector2(
            normalize(gp.x, s.x * 0.714, -s.x * 0.714),
            normalize(gp.z, s.z * 0.714, -s.z * 0.714)
        );
        gp2 = rotate_around_center(gp2, Vector2(0.5, 0.5), get_rotation().y);
        centers.append(gp2);
    }
    myroot->occluders += centers.size();
    if (centers.size() > 0) {
        if (l1->is_visible()) {
            ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter("vignette_centers", centers);
        }
        if (l2->is_visible()) {
            ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter("vignette_centers", centers);
        }
        if (l3->is_visible()) {
            ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter("vignette_centers", centers);
        }
        last_center_size = centers.size();
    } else {
        if (last_center_size != 0) {
            Array dv = dummy_vignettes();
            if (l1->is_visible()) {
                ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter("vignette_centers", dv);
            }
            if (l2->is_visible()) {
                ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter("vignette_centers", dv);
            }
            if (l3->is_visible()) {
                ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter("vignette_centers", dv);
            }
        }
        last_center_size = 0;
    }
}

Vector2 OakTree::rotate_around_center(Vector2 point, Vector2 center, double angle) {
    double s = sin(angle);
	double c = cos(angle);
	Vector2 r = point - center;
	double xnew = r.x * c - r.y * s;
	double ynew = r.x * s + r.y * c;
	r.x = xnew + center.x;
	r.y = ynew + center.y;
	return r;
}

Array OakTree::dummy_vignettes() {
    Array v;
    for (int i = 0; i != 5; i++) {
        v.append(Vector2(100, 100));
    }
    return v;
}

void OakTree::_on_area_3d_area_entered(Area3D* area3d) {
    occluders.emplace(area3d);
}

void OakTree::_on_area_3d_area_exited(Area3D* area3d) {
    occluders.erase(area3d);
}

void OakTree::set_graphics(const std::string& option) {
    if (option == "dynamic") {
        ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter(
            "image_texture", MyTextures::my_textures["1a"]);
        ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter(
            "image_texture", MyTextures::my_textures["1b"]);
        ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter(
            "image_texture", MyTextures::my_textures["1c"]);
    }
}

void OakTree::set_wind_strength(float value, float min_value, float max_value) {
    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter(
            "time_factor", value);
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter(
            "time_factor", value);
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter(
            "time_factor", value);

    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter(
            "sway_frequency", remap(value, min_value, max_value, 0.25, 5));
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter(
            "sway_frequency", remap(value, min_value, max_value, 0.25, 5));
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter(
            "sway_frequency", remap(value, min_value, max_value, 0.25, 5));

    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter(
            "sway_amplitude", remap(value, min_value, max_value, 0.01, 0.09));
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter(
            "sway_amplitude", remap(value, min_value, max_value, 0.03, 0.15));
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter(
            "sway_amplitude", remap(value, min_value, max_value, 0.05, 0.21));

    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter(
            "noise_factor", remap(value, min_value, max_value, 0.005, 0.025));
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter(
            "noise_factor", remap(value, min_value, max_value, 0.005, 0.025));
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter(
            "noise_factor", remap(value, min_value, max_value, 0.005, 0.025));
}


void OakTree::set_wind_rotation(float rads) {
    ((ShaderMaterial*)(*l1->get_material_override()))->set_shader_parameter(
            "rotation", rads - get_rotation().y);
    ((ShaderMaterial*)(*l2->get_material_override()))->set_shader_parameter(
            "rotation", rads - get_rotation().y);
    ((ShaderMaterial*)(*l3->get_material_override()))->set_shader_parameter(
            "rotation", rads - get_rotation().y);
}