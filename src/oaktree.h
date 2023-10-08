#ifndef OAKTREE_H
#define OAKTREE_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <random>

#include "mymain.h"



namespace godot {

class OakTree : public Node3D {
    GDCLASS(OakTree, Node3D)

private:
    MeshInstance3D* l1;
    MeshInstance3D* l2;
    MeshInstance3D* l3;
    CollisionShape3D* collision;
    MyMain* myroot;
    int last_center_size;

protected:
    static void _bind_methods();
    static double get_random_double(double min, double max);

public:
    OakTree();
    ~OakTree();

    void _ready();
    void _process(double delta);

    Array dummy_vignettes();

    std::set<Area3D*> occluders;
    Array centers;

    Vector2 rotate_around_center(Vector2 point, Vector2 center, double angle);
    
    void _on_area_3d_area_entered(Area3D* area3d);
    void _on_area_3d_area_exited(Area3D* area3d);

    void set_graphics(const std::string& option);

    void set_wind_strength(float value, float min_value, float max_value);
    void set_wind_rotation(float rads);

};

}



#endif