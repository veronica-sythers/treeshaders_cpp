extends OptionButton




func _on_item_selected(index: int) -> void:
	for tree in get_tree().get_nodes_in_group("trees"):
		tree.set_graphics(get_item_text(index))
