package cn.alchemy3d.geom
{
	import cn.alchemy3d.polygon.DisplayObject3D;
	
	public class AbstractRenderable
	{
		public var name:String;
		public var instance:DisplayObject3D;
		
		public function AbstractRenderable(instance:DisplayObject3D = null, name:String = "")
		{
			this.name = name;
			this.instance = instance;
		}
	}
}