using Godot;
using System;
using System.ComponentModel;

public partial class agent : Node3D
{

	private NavigationAgent3D navigation_agent;
	private AnimationPlayer anim_player;
	private Node3D trees;
	public bool activated = false;

	//[Signal]
	//public delegate void pathEventHandler(agent arg1);


	public override void _Ready()
	{
		navigation_agent = GetNode<NavigationAgent3D>("NavigationAgent3D");
		anim_player = GetNode<AnimationPlayer>("AnimationPlayer");
		trees = GetNode<Node3D>("../../NavigationRegion3D/trees");
		navigation_agent.PathDesiredDistance = 0.5f;
		navigation_agent.TargetDesiredDistance = 0.5f;
		anim_player.Play("walking");
	}

	public override void _Process(double delta)
	{
		if (activated && navigation_agent.IsNavigationFinished()) {
			// EmitSignal(SignalName.path, (agent)this);
			give_agent_path();
			return;
		}

		Vector3 current_agent_position = this.GlobalPosition;
		Vector3 next_path_position = navigation_agent.GetNextPathPosition();

		Vector3 new_velocity = next_path_position - current_agent_position;
		new_velocity = new_velocity.Normalized();
		new_velocity = new_velocity * 0.25f;

		Vector3 opposite_direction = current_agent_position - next_path_position;
		Vector3 target_position_for_reverse_look = current_agent_position + opposite_direction.Normalized();

		LookAt(target_position_for_reverse_look, new Vector3(0.01f, 1f, 0.01f));

		Position += new Vector3(new_velocity.X * (float)delta, new_velocity.Y * (float)delta, new_velocity.Z * (float)delta);
	}


	public void init(int i)
	{
		GetNode<Area3D>("Area3D").Name = i.ToString();
	}

	private void give_agent_path()
	{
		Godot.Collections.Array<Godot.Node> targets = trees.GetChildren();
		Random rnd = new Random();
		Node3D target = (Node3D)targets[rnd.Next(targets.Count)];
		navigation_agent.TargetPosition = target.GlobalPosition;
	}
}
