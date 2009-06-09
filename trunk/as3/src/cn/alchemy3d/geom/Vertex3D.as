package cn.alchemy3d.geom
{
	import cn.alchemy3d.polygon.Polygon;
	
	import flash.geom.Vector3D;
	
	public class Vertex3D
	{
		public var x:Number;
		public var y:Number;
		public var z:Number;
		
		public var normal:Vector3D;
		public var instance:Polygon;
		
		public function Vertex3D(x:Number = 0, y:Number = 0, z:Number = 0, instance:Polygon = null)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			
			this.normal = new Vector3D();
			this.instance = instance;
		}
		
		public function clone(instance:Polygon = null):Vertex3D
		{
			return new Vertex3D(x, y, z, instance);
		}
		
		public function equals(toCompare:Vertex3D, allFour:Boolean = false):Boolean
		{
			var bool:Boolean = (x == toCompare.x && y == toCompare.y && z == toCompare.z);
			if (allFour) bool = bool && (this.instance == toCompare.instance);
			return bool;
		}
		
		public function reset():void
		{
			x = y = z = 0;
		}
		
		public function toString():String
		{
			return "x:" + x + ", y:" + y + ", z:" + z;
		}
	}
}