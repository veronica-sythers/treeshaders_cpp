extends Node3D


class_name MyTree


var _occluders: Array[Area3D] = []
var _centers: Array[Vector2] = []
var _last_center_size: int = 0
@onready var collision: CollisionShape3D = $Area3D/CollisionShape3D
@onready var my_root = get_parent().get_parent().get_parent()
@onready var l1 = $leaves_1
@onready var l2 = $leaves_2
@onready var l3 = $leaves_3

func _ready() -> void:
	set_graphics("dynamic")
	
	l1.material_override = l1.material_override.duplicate()
	l2.material_override = l2.material_override.duplicate()
	l3.material_override = l3.material_override.duplicate()
	set_high_lod()
	
	l1.material_override.set_shader_parameter("rotation", 90 - rotation.y)
	l2.material_override.set_shader_parameter("rotation", 90 - rotation.y)
	l3.material_override.set_shader_parameter("rotation", 90 - rotation.y)
	
	var to = Utils.randf_range(0.8, 1.2)
	l1.material_override.set_shader_parameter("time_offset", to)
	l2.material_override.set_shader_parameter("time_offset", to) 
	l3.material_override.set_shader_parameter("time_offset", to)

	var dv = dummy_vignettes()
	l1.material_override.set_shader_parameter("vignette_centers", dv)
	l2.material_override.set_shader_parameter("vignette_centers", dv)
	l3.material_override.set_shader_parameter("vignette_centers", dv)
	
	add_to_group("trees")


func _process(delta: float) -> void:
	_centers = []
	for area in _occluders:
		var gp = global_position - area.global_position
		var s = collision.shape.size
		gp = Vector2(
			Utils.normalize(gp.x, s.x * 0.714, -s.x * 0.714),
			Utils.normalize(gp.z, s.z * 0.714, -s.z * 0.714)
		)
		gp = rotate_around_center(gp, Vector2(0.5, 0.5), rotation.y)
		_centers.append(gp)
	if _centers.size() > 0:
		if l1.visible:
			l1.material_override.set_shader_parameter("vignette_centers", _centers)
			my_root.occluders += 1
		if l2.visible:
			l2.material_override.set_shader_parameter("vignette_centers", _centers)
			my_root.occluders += 1
		if l3.visible:
			l3.material_override.set_shader_parameter("vignette_centers", _centers)
			my_root.occluders += 1
		_last_center_size = _centers.size()
	else:
		if _last_center_size != 0:
			var dv = dummy_vignettes()
			if l1.visible:
				l1.material_override.set_shader_parameter("vignette_centers", dv)
				my_root.occluders += 1
			if l2.visible:
				l2.material_override.set_shader_parameter("vignette_centers", dv)
				my_root.occluders += 1
			if l3.visible:
				l3.material_override.set_shader_parameter("vignette_centers", dv)
				my_root.occluders += 1
		_last_center_size = 0


func rotate_around_center(point: Vector2, center: Vector2, angle: float) -> Vector2:
	var s = sin(angle)
	var c = cos(angle)
	point -= center
	var xnew = point.x * c - point.y * s
	var ynew = point.x * s + point.y * c
	point.x = xnew + center.x
	point.y = ynew + center.y
	return point


func dummy_vignettes():
	var v = []
	for _i in range(5):
		v.append(Vector2(100, 100))
	return v


func _on_area_3d_area_entered(area: Area3D) -> void:
	_occluders.append(area)


func _on_area_3d_area_exited(area: Area3D) -> void:
	_occluders.erase(area)


func set_wind_rot(rads: float):
	l1.material_override.set_shader_parameter("rotation", rads - rotation.y)
	l2.material_override.set_shader_parameter("rotation", rads - rotation.y)
	l3.material_override.set_shader_parameter("rotation", rads - rotation.y)


func set_wind_strength(value: float, min: float, max: float):
	l1.material_override.set_shader_parameter("time_factor", value)
	l2.material_override.set_shader_parameter("time_factor", value)
	l3.material_override.set_shader_parameter("time_factor", value)
	
	l1.material_override.set_shader_parameter(
		"sway_frequency", Utils.remap(value, min, max, 0.25, 5))
	l2.material_override.set_shader_parameter(
		"sway_frequency", Utils.remap(value, min, max, 0.25, 5))
	l3.material_override.set_shader_parameter(
		"sway_frequency", Utils.remap(value, min, max, 0.25, 5))
		
	l1.material_override.set_shader_parameter(
		"sway_amplitude", Utils.remap(value, min, max, 0.01, 0.09))
	l2.material_override.set_shader_parameter(
		"sway_amplitude", Utils.remap(value, min, max, 0.03, 0.15))
	l3.material_override.set_shader_parameter(
		"sway_amplitude", Utils.remap(value, min, max, 0.05, 0.21))
		
	l1.material_override.set_shader_parameter(
		"noise_factor", Utils.remap(value, min, max, 0.005, 0.025))
	l2.material_override.set_shader_parameter(
		"noise_factor", Utils.remap(value, min, max, 0.005, 0.025))
	l3.material_override.set_shader_parameter(
		"noise_factor", Utils.remap(value, min, max, 0.005, 0.025))


func set_low_lod() -> void:
	l1.transparency = 0.0
	l1.cast_shadow = false
	$greendot.visible = true
	$greendot.transparency = 1.0
	$greendot.cast_shadow = false
	var tween_1 = get_tree().create_tween()
	tween_1.tween_property($leaves_1, "transparency", 1.0, 0.25)
	var tween_2 = get_tree().create_tween()
	tween_2.tween_property($greendot, "transparency", 0.0, 0.25)
	tween_2.tween_callback(set_all_invisible)
	$greendot.rotation.y = -rotation.y


func set_mid_from_high_lod() -> void:
	l2.transparency = 0.0
	l3.transparency = 0.0
	$dirt.transparency = 0.0
	l1.cast_shadow = false
	l2.cast_shadow = false
	l3.cast_shadow = false
	var tween_1 = get_tree().create_tween()
	tween_1.tween_property(l2, "transparency", 1.0, 0.25)
	var tween_2 = get_tree().create_tween()
	tween_2.tween_property(l3, "transparency", 1.0, 0.25)
	var tween_3 = get_tree().create_tween()
	tween_3.tween_property($dirt, "transparency", 1.0, 0.25)
	tween_3.tween_callback(set_invisible)
	$greendot.visible = false


func set_mid_from_low_lod() -> void:
	l1.transparency = 1.0
	l1.visible = true
	var tween_1 = get_tree().create_tween()
	tween_1.tween_property(l1, "transparency", 0.0, 0.25)
	tween_1.tween_callback(set_mid_from_low_callback)
	$greendot.visible = false


func set_high_lod() -> void:
	l2.transparency = 1.0
	l3.transparency = 1.0
	$dirt.transparency = 1.0
	l2.visible = true
	l3.visible = true
	$dirt.visible = true
	l2.cast_shadow = false
	l3.cast_shadow = false
	var tween_1 = get_tree().create_tween()
	tween_1.tween_property(l2, "transparency", 0.0, 0.25)
	var tween_2 = get_tree().create_tween()
	tween_2.tween_property(l3, "transparency", 0.0, 0.25)
	var tween_3 = get_tree().create_tween()
	tween_3.tween_property($dirt, "transparency", 0.0, 0.25)
	tween_3.tween_callback(set_shadow_on)
	$greendot.visible = false


func set_mid_from_low_callback() -> void:
	$greendot.visible = false


func set_all_invisible() -> void:
	$leaves_1.visible = false


func set_invisible() -> void:
	l2.visible = false
	l3.visible = false
	$dirt.visible = false


func set_shadow_on() -> void:
	l1.cast_shadow = true
	l2.cast_shadow = true
	l3.cast_shadow = true


func set_graphics(option: String):
	match option:
		"original": 
			l1.material_override.set_shader_parameter("image_texture", Textures.bot_a)
			l2.material_override.set_shader_parameter("image_texture", Textures.mid_a)
			l3.material_override.set_shader_parameter("image_texture", Textures.top_a)
		"speed_tree": 
			l1.material_override.set_shader_parameter("image_texture", Textures.bot_b)
			l2.material_override.set_shader_parameter("image_texture", Textures.mid_b)
			l3.material_override.set_shader_parameter("image_texture", Textures.top_b)
		"speed_tree_expanded": 
			l1.material_override.set_shader_parameter("image_texture", Textures.bot_c)
			l2.material_override.set_shader_parameter("image_texture", Textures.mid_c)
			l3.material_override.set_shader_parameter("image_texture", Textures.top_c)
		"speed_tree_grayscale": 
			l1.material_override.set_shader_parameter("image_texture", Textures.bot_d)
			l2.material_override.set_shader_parameter("image_texture", Textures.mid_d)
			l3.material_override.set_shader_parameter("image_texture", Textures.top_d)
		"speed_tree_greenscale": 
			l1.material_override.set_shader_parameter("image_texture", Textures.bot_e)
			l2.material_override.set_shader_parameter("image_texture", Textures.mid_e)
			l3.material_override.set_shader_parameter("image_texture", Textures.top_e)
		"dynamic":
			match randi() % 4:
				0:
					l1.material_override.set_shader_parameter("image_texture", Textures.one_a)
					l2.material_override.set_shader_parameter("image_texture", Textures.one_b)
					l3.material_override.set_shader_parameter("image_texture", Textures.one_c)
				1:
					l1.material_override.set_shader_parameter("image_texture", Textures.two_a)
					l2.material_override.set_shader_parameter("image_texture", Textures.two_b)
					l3.material_override.set_shader_parameter("image_texture", Textures.two_c)
				2:
					l1.material_override.set_shader_parameter("image_texture", Textures.thr_a)
					l2.material_override.set_shader_parameter("image_texture", Textures.thr_b)
					l3.material_override.set_shader_parameter("image_texture", Textures.thr_c)
				3:
					l1.material_override.set_shader_parameter("image_texture", Textures.fou_a)
					l2.material_override.set_shader_parameter("image_texture", Textures.fou_b)
					l3.material_override.set_shader_parameter("image_texture", Textures.fou_c)
