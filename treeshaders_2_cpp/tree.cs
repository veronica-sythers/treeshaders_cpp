using Godot;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Diagnostics;
using System.Linq;

public partial class tree : Node3D
{

	private Texture2D bot_a = ResourceLoader.Load<Texture2D>("res://tree_big.png");
	private Texture2D mid_a = ResourceLoader.Load<Texture2D>("res://tree_small.png");
	private Texture2D top_a = ResourceLoader.Load<Texture2D>("res://tree_tiny.png");
	private Texture2D bot_b = ResourceLoader.Load<Texture2D>("res://bot_b.png");
	private Texture2D mid_b = ResourceLoader.Load<Texture2D>("res://mid_b.png");
	private Texture2D top_b = ResourceLoader.Load<Texture2D>("res://top_b.png");
	private Texture2D bot_c = ResourceLoader.Load<Texture2D>("res://bot_c.png");
	private Texture2D mid_c = ResourceLoader.Load<Texture2D>("res://mid_c.png");
	private Texture2D top_c = ResourceLoader.Load<Texture2D>("res://top_c.png");
	private Texture2D bot_d = ResourceLoader.Load<Texture2D>("res://bot_d.png");
	private Texture2D mid_d = ResourceLoader.Load<Texture2D>("res://mid_d.png");
	private Texture2D top_d = ResourceLoader.Load<Texture2D>("res://top_d.png");
	private Texture2D bot_e = ResourceLoader.Load<Texture2D>("res://bot_e.png");
	private Texture2D mid_e = ResourceLoader.Load<Texture2D>("res://mid_e.png");
	private Texture2D top_e = ResourceLoader.Load<Texture2D>("res://top_e.png");

	private Texture2D  one_a = ResourceLoader.Load<Texture2D>("res://1a.png");
	private Texture2D  one_b = ResourceLoader.Load<Texture2D>("res://1b.png");
	private Texture2D  one_c = ResourceLoader.Load<Texture2D>("res://1c.png");
	private Texture2D  two_a = ResourceLoader.Load<Texture2D>("res://2a.png");
	private Texture2D  two_b = ResourceLoader.Load<Texture2D>("res://2b.png");
	private Texture2D  two_c = ResourceLoader.Load<Texture2D>("res://2c.png");
	private Texture2D  thr_a = ResourceLoader.Load<Texture2D>("res://3a.png");
	private Texture2D  thr_b = ResourceLoader.Load<Texture2D>("res://3b.png");
	private Texture2D  thr_c = ResourceLoader.Load<Texture2D>("res://3c.png");
	private Texture2D  fou_a = ResourceLoader.Load<Texture2D>("res://4a.png");
	private Texture2D  fou_b = ResourceLoader.Load<Texture2D>("res://4b.png");
	private Texture2D  fou_c = ResourceLoader.Load<Texture2D>("res://4c.png");


	private MeshInstance3D l1;
	private MeshInstance3D l2;
	private MeshInstance3D l3;
	private ShaderMaterial l1m;
	private ShaderMaterial l2m;
	private ShaderMaterial l3m;
	private Node3D my_root;
	private CollisionShape3D collision;
	private Dictionary<string, Area3D> _occluders = new Dictionary<string, Area3D>();
	private Godot.Vector2[] _centers;
	int _last_center_size = 0;

	public override void _Ready()
	{

		l1 = GetNode<MeshInstance3D>("leaves_1");
		l2 = GetNode<MeshInstance3D>("leaves_2");
		l3 = GetNode<MeshInstance3D>("leaves_3");
		my_root = GetParent<Node3D>().GetParent<NavigationRegion3D>().GetParent<Node3D>();
		collision = GetNode<CollisionShape3D>("Area3D/CollisionShape3D");

		l1.MaterialOverride = (ShaderMaterial)(l1.MaterialOverride.Duplicate());
		l2.MaterialOverride = (ShaderMaterial)(l2.MaterialOverride.Duplicate());
		l3.MaterialOverride = (ShaderMaterial)(l3.MaterialOverride.Duplicate());
		set_high_lod();

		l1m = (ShaderMaterial)l1.MaterialOverride;
		l2m = (ShaderMaterial)l2.MaterialOverride;
		l3m = (ShaderMaterial)l3.MaterialOverride;

		l1m.SetShaderParameter("rotation", 90 - Rotation.Y);
		l2m.SetShaderParameter("rotation", 90 - Rotation.Y);
		l3m.SetShaderParameter("rotation", 90 - Rotation.Y);

		float to = 0.8f + GD.Randf() * (1.2f - 0.8f);
		l1m.SetShaderParameter("time_offset", to);
		l2m.SetShaderParameter("time_offset", to);
		l3m.SetShaderParameter("time_offset", to);

		Godot.Vector2[] dv = dummy_vignettes();
		l1m.SetShaderParameter("vignette_centers", dv);
		l2m.SetShaderParameter("vignette_centers", dv);
		l3m.SetShaderParameter("vignette_centers", dv);

		set_graphics("dynamic");
		AddToGroup("trees");
	}

	public override void _Process(double delta)
	{
		/*
			Send a list of Vector2s into the shader code. 
			Each vector2 represents an occluding area3d
			for which a transparent vignette effect
			should be created.
		*/
		_centers = new Godot.Vector2[_occluders.Count];
		int idx = 0;
		foreach (KeyValuePair<string, Area3D> entry in _occluders)
    	{
			Godot.Vector3 gp = GlobalPosition - entry.Value.GlobalPosition;
			Godot.Vector3 s = ((BoxShape3D)collision.Shape).Size;
			Godot.Vector2 gp2 = new Godot.Vector2(
				normalize(gp.X, s.X * 0.5f, -s.X * 0.5f),
				normalize(gp.Z, s.Z * 0.5f, -s.Z * 0.5f)
			);
			gp2 = rotate_around_center(gp2, new Godot.Vector2(0.5f, 0.5f), Rotation.Y);
			_centers[idx++] = gp2;
			((main)my_root).occluders += 1;
		}
		if (_centers.Count() > 0) {
			if (l1.Visible) {
				l1m.SetShaderParameter("vignette_centers", _centers);
				((main)my_root).occluders += 1;
			}
			if (l2.Visible) {
				l2m.SetShaderParameter("vignette_centers", _centers);
				((main)my_root).occluders += 1;
			}
			if (l3.Visible) {
				l3m.SetShaderParameter("vignette_centers", _centers);
				((main)my_root).occluders += 1;
			}
		} else {
			if (_last_center_size != 0) {
				Godot.Vector2[] dv = dummy_vignettes();
			if (l1.Visible) {
				l1m.SetShaderParameter("vignette_centers", dv);
				((main)my_root).occluders += 1;
			}
			if (l2.Visible) {
				l2m.SetShaderParameter("vignette_centers", dv);
				((main)my_root).occluders += 1;
			}
			if (l3.Visible) {
				l3m.SetShaderParameter("vignette_centers", dv);
				((main)my_root).occluders += 1;
			}
			}
		}
	}

	private Godot.Vector2[] dummy_vignettes() {
		Godot.Vector2[] v = new Godot.Vector2[5];
		for (int i = 0; i != 5; i++) {
			v[i] = new Godot.Vector2(100, 100);
		}
		return v;
	}

	private float normalize(float value, float min_original, float max_original) 
	{
		return (value - min_original) / (max_original - min_original);
	}

	private Godot.Vector2 rotate_around_center(Godot.Vector2 point, Godot.Vector2 center, float angle)
  {
		float s = (float)Math.Sin(angle);
		float c = (float)Math.Cos(angle);
		point -= center;
		float xnew = point.X * c - point.Y * s;
		float ynew = point.X * s - point.Y * c;
		point.X = xnew;
		point.Y = ynew;
		return point;
	}

	public void _on_area_3d_area_entered(Area3D area)
	{
		_occluders.Add(area.Name, area);
	}

	public void _on_area_3d_area_exited(Area3D area)
	{
		_occluders.Remove(area.Name);
	}

	private void set_wind_rot(float rads)
  	{
		l1m.SetShaderParameter("rotation", rads - Rotation.Y);
		l2m.SetShaderParameter("rotation", rads - Rotation.Y);
		l3m.SetShaderParameter("rotation", rads - Rotation.Y);
	}

	private float remap(float value, float old_min, float old_max, float new_min, float new_max) 
	{
		return new_min + ((value - old_min) / (old_max - old_min)) * (new_max - new_min);
	}

	private void set_wind_strength(float value, float min, float max)
	{
		l1m.SetShaderParameter("time_factor", value);
		l2m.SetShaderParameter("time_factor", value);
		l3m.SetShaderParameter("time_factor", value);

		l1m.SetShaderParameter(
			"sway_frequency", remap(value, min, max, 0.25f, 5f));
		l2m.SetShaderParameter(
			"sway_frequency", remap(value, min, max, 0.25f, 5f));
		l3m.SetShaderParameter(
			"sway_frequency", remap(value, min, max, 0.25f, 5f));

		l1m.SetShaderParameter(
			"sway_amplitude", remap(value, min, max, 0.01f, 0.09f));
		l2m.SetShaderParameter(
			"sway_amplitude", remap(value, min, max, 0.03f, 0.15f));
		l3m.SetShaderParameter(
			"sway_amplitude", remap(value, min, max, 0.05f, 0.21f));

		l1m.SetShaderParameter(
			"noise_factor", remap(value, min, max, 0.005f, 0.025f));
		l2m.SetShaderParameter(
			"noise_factor", remap(value, min, max, 0.005f, 0.025f));
		l3m.SetShaderParameter(
			"noise_factor", remap(value, min, max, 0.005f, 0.025f));
	}

	private void set_mid_from_high_lod()
	{
		l2.Transparency = 0.0f;
		l3.Transparency = 0.0f;
		GetNode<MeshInstance3D>("dirt").Transparency = 0.0f;
		l1.CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		l2.CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		l3.CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		Tween tween_1 = GetTree().CreateTween();
		tween_1.TweenProperty(l2, "transparency", 1.0, 0.2f);
		Tween tween_2 = GetTree().CreateTween();
		tween_2.TweenProperty(l3, "transparency", 1.0, 0.2f);
		Tween tween_3 = GetTree().CreateTween();
		tween_3.TweenProperty(GetNode<MeshInstance3D>("dirt"), "transparency", 1.0, 0.2);
		tween_3.TweenCallback(Callable.From(set_invisible));
		GetNode<Sprite3D>("greendot").Visible = false;
		l1.Visible = true;
		l1.Transparency = 0.0f;
	}

	private void set_low_lod() 
	{
		l1.Transparency = 0.0f;
		l1.CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		GetNode<Sprite3D>("greendot").Visible = true;
		GetNode<Sprite3D>("greendot").Transparency = 1.0f;
		GetNode<Sprite3D>("greendot").CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		Tween tween_1 = GetTree().CreateTween();
		tween_1.TweenProperty(l1, "transparency", 1.0, 0.25);
		Tween tween_2 = GetTree().CreateTween();
		tween_2.TweenProperty(GetNode<Sprite3D>("greendot"), "transparency", 0.0, 0.25);
		GetNode<Sprite3D>("greendot").Rotation = new Godot.Vector3(-90, -Rotation.Y, 0);
		tween_2.TweenCallback(Callable.From(set_all_invisible));
	}

	
	private void set_mid_from_low_lod()
	{
		l1.Transparency = 1.0f;
		l1.Visible = true;
		Tween tween_1 = GetTree().CreateTween();
		tween_1.TweenProperty(l1, "transparency", 0.0, 0.2f);
		tween_1.TweenCallback(Callable.From(set_mid_from_low_callback));
		GetNode<Sprite3D>("greendot").Visible = false;
	}

	
	private void set_mid_from_low_callback() {
		GetNode<Sprite3D>("greendot").Visible = false;
	}

	private void set_high_lod()
	{
		l2.Transparency = 1.0f;
		l3.Transparency = 1.0f;
		GetNode<MeshInstance3D>("dirt").Transparency = 1.0f;
		l2.Visible = true;
		l3.Visible = true;
		GetNode<MeshInstance3D>("dirt").Visible = false;
		l2.CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		l3.CastShadow = GeometryInstance3D.ShadowCastingSetting.Off;
		Tween tween_1 = GetTree().CreateTween();
		tween_1.TweenProperty(l2, "transparency", 0.0, 0.2f);
		Tween tween_2 = GetTree().CreateTween();
		tween_2.TweenProperty(l3, "transparency", 0.0, 0.2f);
		Tween tween_3 = GetTree().CreateTween();
		tween_3.TweenProperty(GetNode<MeshInstance3D>("dirt"), "transparency", 0.0, 0.2);
		tween_3.TweenCallback(Callable.From(set_shadow_on));
		GetNode<Sprite3D>("greendot").Visible = false;
	}

	private void set_invisible()
 	 {
		l2.Visible = false;
		l3.Visible = false;
		GetNode<MeshInstance3D>("dirt").Visible = false;
	}
	private void set_all_invisible() 
	{
		l1.Visible = false;
	}

	private void set_shadow_on()
	{
		l1.CastShadow = GeometryInstance3D.ShadowCastingSetting.On;
		l2.CastShadow = GeometryInstance3D.ShadowCastingSetting.On;
		l3.CastShadow = GeometryInstance3D.ShadowCastingSetting.On;
	}

	private void set_graphics(String option)
	{
		Random rnd = new Random();
		switch (option)
		{
			case "original":
				l1m.SetShaderParameter("image_texture", bot_a);
				l2m.SetShaderParameter("image_texture", mid_a);
				l3m.SetShaderParameter("image_texture", top_a);
				break;
			case "speed_tree":
				l1m.SetShaderParameter("image_texture", bot_b);
				l2m.SetShaderParameter("image_texture", mid_b);
				l3m.SetShaderParameter("image_texture", top_b);
				break;
			case "speed_tree_expanded":
				l1m.SetShaderParameter("image_texture", bot_c);
				l2m.SetShaderParameter("image_texture", mid_c);
				l3m.SetShaderParameter("image_texture", top_c);
				break;
			case "speed_tree_grayscale":
				l1m.SetShaderParameter("image_texture", bot_d);
				l2m.SetShaderParameter("image_texture", mid_d);
				l3m.SetShaderParameter("image_texture", top_d);
				break;
			case "speed_tree_greenscale":
				l1m.SetShaderParameter("image_texture", bot_e);
				l2m.SetShaderParameter("image_texture", mid_e);
				l3m.SetShaderParameter("image_texture", top_e);
				break;
			case "dynamic":
				switch (rnd.Next(0, 4)) {
					case 0:
						l1m.SetShaderParameter("image_texture", one_a);
						l2m.SetShaderParameter("image_texture", one_b);
						l3m.SetShaderParameter("image_texture", one_c);
						break;
					case 1:
						l1m.SetShaderParameter("image_texture", two_a);
						l2m.SetShaderParameter("image_texture", two_b);
						l3m.SetShaderParameter("image_texture", two_c);
						break;
					case 2:
						l1m.SetShaderParameter("image_texture", thr_a);
						l2m.SetShaderParameter("image_texture", thr_b);
						l3m.SetShaderParameter("image_texture", thr_c);
						break;
					case 3:
						l1m.SetShaderParameter("image_texture", fou_a);
						l2m.SetShaderParameter("image_texture", fou_b);
						l3m.SetShaderParameter("image_texture", fou_c);
						break;
				}
				break;
		}
	}
}
