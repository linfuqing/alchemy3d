package cn.alchemy3d.geom
{
	import cn.alchemy3d.objects.Entity;
	
	import flash.geom.Vector3D;
	
	public class Vertex3D
	{
		public var x:Number;
		public var y:Number;
		public var z:Number;
		public var w:Number;
		
		public var normal:Vector3D;
		public var instance:Entity;
		
		public function Vertex3D(x:Number = 0, y:Number = 0, z:Number = 0, w:Number = 1, instance:Entity = null)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
			
			this.normal = new Vector3D();
			this.instance = instance;
		}
		
		public function clone(instance:Entity = null):Vertex3D
		{
			return new Vertex3D(x, y, z, w, instance);
		}
		
		public function equals(toCompare:Vertex3D, allFour:Boolean = false):Boolean
		{
			var bool:Boolean = (x == toCompare.x && y == toCompare.y && z == toCompare.z && w == toCompare.w);
			if (allFour) bool = bool && (this.instance == toCompare.instance);
			return bool;
		}
		
		public function reset():void
		{
			x = y = z = 0;
			w = 1;
		}
		
		public function toString():String
		{
			return "x:" + x + ", y:" + y + ", z:" + z + ", w:" + w;
		}
	}
}