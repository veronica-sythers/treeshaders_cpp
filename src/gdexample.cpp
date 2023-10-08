#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GDExample::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_amplitude"), &GDExample::get_amplitude);
    ClassDB::bind_method(D_METHOD("set_amplitude", "p_amplitude"), &GDExample::set_amplitude);
    ClassDB::add_property("GDExample", PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");
    ClassDB::bind_method(D_METHOD("get_time_passed"), &GDExample::get_time_passed);
    ClassDB::bind_method(D_METHOD("set_time_passed", "p_time_passed"), &GDExample::set_time_passed);
    ClassDB::add_property("GDExample", PropertyInfo(Variant::FLOAT, "time_passed"), "set_time_passed", "get_time_passed");
}

GDExample::GDExample() {
    // Initialize any variables here.
    time_passed = 0.0;
    amplitude = 10.0;
}

GDExample::~GDExample() {
    // Add your cleanup here.
}

void GDExample::_process(double delta) {
    time_passed += delta;

    Vector2 new_position = Vector2(
        amplitude + (amplitude * sin(time_passed * 2.0)),
        amplitude + (amplitude * cos(time_passed * 1.5))
    );

    set_position(new_position);
}

void GDExample::set_amplitude(const double p_amplitude) {
    amplitude = p_amplitude;
}

double GDExample::get_amplitude() const {
    return amplitude;
}

void GDExample::set_time_passed(const double p_time_passed) {
    time_passed = p_time_passed;
}

double GDExample::get_time_passed() const {
    return time_passed;
}