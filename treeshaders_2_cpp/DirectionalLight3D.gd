extends DirectionalLight3D


func _on_x_changed(value: float) -> void:
	rotation.x = value


func _on_y_changed(value: float) -> void:
	rotation.y = value
