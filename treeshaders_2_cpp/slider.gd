extends PanelContainer


func _on_value_changed(value: float) -> void:
	$hbox/left/Label.text = str(value)


func _on_wind_rot_changed(value: float) -> void:
	for tree in get_tree().get_nodes_in_group("trees"):
		tree.set_wind_rotation(-value)


func _on_arrow_changed(value: float) -> void:
	$hbox/left/arrow.rotation = value - deg_to_rad(90)


func _onwind_strength_changed(value: float) -> void:
	var hs = $hbox/middle/HSlider
	for tree in get_tree().get_nodes_in_group("trees"):
		tree.set_wind_strength(value, hs.min_value, hs.max_value)

