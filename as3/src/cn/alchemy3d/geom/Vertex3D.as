package cn.alchemy3d.geom
{	
	import flash.geom.Vector3D;
	
	public class Vertex3D
	{
		public var x:Number;
		public var y:Number;
		public var z:Number;
		public var w:Number;
		
		public var normal:Vector3D;
		
		public var pointer:uint;
		
		public function Vertex3D(x:Number = 0, y:Number = 0, z:Number = 0, w:Number = 1)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
			
			this.normal = new Vector3D();
		}
		
		public function clone():Vertex3D
		{
			return new Vertex3D(x, y, z, w);
		}
		
		public function equals(toCompare:Vertex3D, allFour:Boolean = false):Boolean
		{
			var bool:Boolean = (x == toCompare.x && y == toCompare.y && z == toCompare.z);
			if (allFour) bool = bool && (w == toCompare.w);
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