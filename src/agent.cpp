#include "agent.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Agent::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_activated"), &Agent::get_activated);
    ClassDB::bind_method(D_METHOD("set_activated", "p_activated"), &Agent::set_activated);
    ClassDB::add_property("Agent", PropertyInfo(Variant::BOOL, "activated"), "set_activated", "get_activated");

    ADD_SIGNAL(MethodInfo("path", PropertyInfo(Variant::OBJECT, "node")));

    ClassDB::bind_method(D_METHOD("init"), &Agent::init);
    ClassDB::bind_method(D_METHOD("goto_target", "node"), &Agent::goto_target);
}

Agent::Agent() {
    // Initialize any variables here.
}

Agent::~Agent() {
    // Add your cleanup here.
}

void Agent::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    if (activated && navigation_agent->is_navigation_finished()) {
        emit_signal("path", this);
    }

    Vector3 next_path_position = navigation_agent->get_next_path_position();

    if (next_path_position == last_path_position) {
        return;
    }

    Vector3 new_velocity = next_path_position - get_global_position();
    new_velocity = new_velocity.normalized() * 0.25;

    Vector3 opposite_direction = get_global_position() - next_path_position;
    Vector3 target_position_for_reverse_look = get_global_position() + opposite_direction.normalized();

    look_at(target_position_for_reverse_look, Vector3(0.01, 1, 0.01));

    set_position(get_position() + new_velocity * delta);

    last_path_position = get_global_position();
}

void Agent::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    navigation_agent = get_node<NavigationAgent3D>("NavigationAgent3D");
    navigation_agent->set_path_desired_distance(0.5);
    navigation_agent->set_target_desired_distance(0.5);
    anim_player = get_node<AnimationPlayer>("AnimationPlayer");
    anim_player->play("walking");
}

void Agent::set_activated(const double p_activated) {
    activated = p_activated;
}

bool Agent::get_activated() const {
    return activated;
}

void Agent::goto_target(Node3D* node) {
//void Agent::goto_target(const Vector3& target) {
    navigation_agent->set_target_position(node->get_global_position());
    //navigation_agent->set_target_position(target);
}

void Agent::init(int i) {
    uniq = i;
}