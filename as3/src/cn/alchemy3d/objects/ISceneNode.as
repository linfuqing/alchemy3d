package cn.alchemy3d.objects
{
	public interface ISceneNode
	{
		function initialize(scenePtr:uint, parentPtr:uint):void;
		function allotPtr(ps:Array):void;
	}
}