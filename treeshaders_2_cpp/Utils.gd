extends Node


var rng = RandomNumberGenerator.new()


func remap(value: float, old_min: float, old_max: float, new_min: float, new_max: float):
	return new_min + ((value - old_min) / (old_max - old_min)) * (new_max - new_min)


func randf_range(min: float, max: float) -> float:
	return rng.randf_range(min, max)


func normalize(value: float, min_original: float, max_original: float) -> float:
	return (value - min_original) / (max_original - min_original)
