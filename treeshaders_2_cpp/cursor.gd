extends Area3D

@onready var camera: Camera3D = $"../Camera3D"


func _process(delta: float) -> void:
	pass


func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		var world_position = camera.project_position(event.position, 5)
		global_position = world_position
