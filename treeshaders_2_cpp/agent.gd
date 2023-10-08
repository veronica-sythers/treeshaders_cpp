extends Node3D


class_name MyAgent


@onready var navigation_agent: NavigationAgent3D = $NavigationAgent3D
@onready var anim_player: AnimationPlayer = $AnimationPlayer
var activated = false
var last_path_position: Vector3 = Vector3.ZERO
var uniq

signal path(agent: MyAgent)


func _ready() -> void:
	navigation_agent.path_desired_distance = 0.5
	navigation_agent.target_desired_distance = 0.5
	anim_player.play("walking")


func goto_target(target):
	navigation_agent.set_target_position(target.global_position)


func _process(delta):
	if activated and navigation_agent.is_navigation_finished():
		emit_signal("path", self)

	var next_path_position: Vector3 = navigation_agent.get_next_path_position()
	
	if next_path_position == last_path_position:
		return
	
	var new_velocity: Vector3 = next_path_position - global_position
	new_velocity = new_velocity.normalized() * 0.25

	var opposite_direction: Vector3 = global_position - next_path_position
	var target_position_for_reverse_look: Vector3 = global_position + opposite_direction.normalized()

	look_at(target_position_for_reverse_look, Vector3(0.01, 1, 0.01))

	position += new_velocity * delta
	
	last_path_position = global_position


func init(i):
	uniq = str(i)
