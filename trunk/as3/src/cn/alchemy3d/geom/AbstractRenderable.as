package cn.alchemy3d.geom
{
	import cn.alchemy3d.objects.Entity;
	
	public class AbstractRenderable
	{
		public var name:String;
		public var instance:Entity;
		
		public function AbstractRenderable(instance:Entity = null, name:String = "")
		{
			this.name = name;
			this.instance = instance;
		}
	}
}