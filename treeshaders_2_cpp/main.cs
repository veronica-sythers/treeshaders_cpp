using Godot;
using System;
using System.Threading.Tasks;

public partial class main : Node3D
{

	private PackedScene tree_scene = ResourceLoader.Load<PackedScene>("res://tree.tscn");
	private PackedScene agent_scene = ResourceLoader.Load<PackedScene>("res://agent.tscn");
	public int occluders;
	private static Random random = new Random();

	private Label fps;
	private Label trees;
	private Label occluders_label;


	public override async void _Ready()
	{

		fps = GetNode<Label>("Control/fps");
		trees = GetNode<Label>("Control/trees");
		occluders_label = GetNode<Label>("Control/occluders");

		NoiseTexture2D texture = new NoiseTexture2D();
		texture.Noise = new FastNoiseLite();
		await Task.Delay(1000);  // Wait for texture to load
		Image image = texture.GetImage();

		int max = 100;
		for (int x = 0; x != max; x++) {
			for (int y = 0; y != max; y++) {
				float xx = remap(x, 0, max, 0, 512);
				float yy = remap(y, 0, max, 0, 512);
				if (texture.Noise.GetNoise2D(xx, yy) > 0.0) {  // place trees whereever noise is above 0.0
					float r = 0.25f;
					Vector3 spawn_loc = new Vector3(
						remap(x, 0, max, -36 + Randf(-r, r), 36 + Randf(-r, r)),
						0,
						remap(y, 0, max, -36 + Randf(-r, r), 36 + Randf(-r, r))
					);
					add_tree(spawn_loc);
				}
			}
		}
		GetNode<NavigationRegion3D>("NavigationRegion3D").BakeNavigationMesh();
	}

	public static float Randf(float min, float max)
    {
        return (float)(min + (random.NextDouble() * (max - min)));
    }

	public override void _Process(double delta)
	{
		fps.Text = "FPS: " + Engine.GetFramesPerSecond().ToString();
		trees.Text = "Trees: " + GetNode<Node3D>("NavigationRegion3D/trees").GetChildCount().ToString();
		occluders_label.Text = "Occluders: " + occluders.ToString();
		occluders = 0;
	}

	public void set_cam_size(float size)
  	{
		GetNode<Label>("Control/cam_size").Text = "Cam Size: " + Math.Round(size).ToString();
	}

	private float remap(float value, float old_min, float old_max, float new_min, float new_max)
	{
		return new_min + ((value - old_min) / (old_max - old_min)) * (new_max - new_min);
	}

	private void add_tree(Vector3 spawn_loc)
  	{
		tree tree = (tree)tree_scene.Instantiate();
		tree.Position = spawn_loc;
		tree.Rotation = new Vector3(tree.Rotation.X, GD.Randf() * 100.0f, tree.Rotation.Z);
		GetNode<Node3D>("NavigationRegion3D/trees").AddChild(tree);
	}

	public void _on_navigation_region_3d_bake_finished()
	{	
		// add a thousand navigating agents
		for (int i = 0; i < 1000; i++)
    	{
			agent agent = (agent)agent_scene.Instantiate();
			agent.Position = new Vector3(-10.0f + GD.Randf() * (20.0f), 0.1f, -10.0f + GD.Randf() * (20.0f));
			agent.activated = true;
			agent.init(i);
			GetNode<Node3D>("agents").AddChild(agent);
		}
  }

}
