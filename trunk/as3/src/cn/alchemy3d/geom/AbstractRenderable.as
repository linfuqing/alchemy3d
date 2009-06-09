package cn.alchemy3d.geom
{
	import cn.alchemy3d.polygon.Polygon;
	
	public class AbstractRenderable
	{
		public var name:String;
		public var instance:Polygon;
		
		public function AbstractRenderable(instance:Polygon = null, name:String = "")
		{
			this.name = name;
			this.instance = instance;
		}
	}
}