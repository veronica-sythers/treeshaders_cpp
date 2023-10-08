#ifndef AGENT_H
#define AGENT_H

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class Agent : public Node3D {
    GDCLASS(Agent, Node3D)

private:

    NavigationAgent3D* navigation_agent;
    AnimationPlayer* anim_player;
    bool activated;
    Vector3 last_path_position;
    int uniq;

protected:
    static void _bind_methods();

public:
    Agent();
    ~Agent();

    void _process(double delta);
	
    void _ready();

	void set_activated(const double p_activated);
	bool get_activated() const;

    void goto_target(Node3D* node);

    void init(int i );

};

}

#endif