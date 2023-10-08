extends Node3D

const tree_scene = preload("res://tree.tscn")
const agent_scene = preload("res://agent.tscn")
var occluders: int = 0

func _process(delta: float) -> void:
	$Control/fps.text = "FPS: %s" % str(Engine.get_frames_per_second())
	$Control/trees.text = "Trees: %s" % str($NavigationRegion3D/trees.get_child_count())
	$Control/occluders.text = "Occluders: %s" % str(occluders)
	occluders = 0

func set_cam_size(size: float) -> void:
	$Control/cam_size.text = "CAM SIZE: %s" % round(size)

func round_to_dec(num, digit):
	return round(num * pow(10.0, digit)) / pow(10.0, digit)

func _ready() -> void:
	var texture = NoiseTexture2D.new()
	texture.noise = FastNoiseLite.new()
	await texture.changed
	var image = texture.get_image()

	var max = 100
	for x in range(max):
		for y in range(max):
			var xx = Utils.remap(x, 0, max, 0, 512)
			var yy = Utils.remap(y, 0, max, 0, 512)
			if texture.noise.get_noise_2d(xx, yy) > 0.0:
				var r = 0.25
				var spawn_loc: Vector3 = Vector3(
					Utils.remap(x, 0, max, -36 + randf_range(-r, r), 36 + randf_range(-r, r)), 
					0, 
					Utils.remap(y, 0, max, -36 + randf_range(-r, r), 36 + randf_range(-r, r))
				)
				add_tree(spawn_loc)
	
	$NavigationRegion3D.bake_navigation_mesh()

func add_tree(pos: Vector3) -> void:
	var tree = tree_scene.instantiate()
	tree.position = pos
	tree.rotation.y = Utils.randf_range(-100, 100)
	$NavigationRegion3D/trees.add_child(tree)

func _on_navigation_region_3d_bake_finished() -> void:
	for i in range(1500):
		var agent = agent_scene.instantiate()
		agent.position = Vector3(Utils.randf_range(-20, 20), 0.1, Utils.randf_range(-20, 20))
		agent.activated = true
		agent.path.connect(give_agent_path)
		agent.init(i)
		$agents.add_child(agent)

func give_agent_path(agent: MyAgent) -> void:
	var targets = $NavigationRegion3D/trees.get_children()
	var target = targets[randi() % targets.size()]
	agent.goto_target(target)
