package cn.alchemy3d.geom
{	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	public class Vertex3D extends Pointer
	{
		private var _x:Number;
		private var _y:Number;
		private var _z:Number;
		
		//private var xPointer:uint;
		//private var yPointer:uint;
		//private var zPointer:uint;
		
		public function set x( x:Number ):void
		{
			_x = x;
			
			Library.memory.position = _pointer;
			
			Library.memory.writeFloat( x );
		}
		
		public function get x():Number
		{
			return _x;
		}
		
		public function set y( y:Number ):void
		{
			_y = y;
			
			Library.memory.position = _pointer + Library.intTypeSize;
			
			Library.memory.writeFloat( y );
		}
		
		public function get y():Number
		{
			return _y;
		}
		
		public function set z( z:Number ):void
		{
			_z = z;
			
			Library.memory.position = _pointer + Library.intTypeSize * 2;
			
			Library.memory.writeFloat( z );
		}
		
		public function get z():Number
		{
			return _z;
		}
		
		public function Vertex3D(x:Number = 0, y:Number = 0, z:Number = 0, w:Number = 1)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}
		
		internal function setPointer( pointer:uint ):void
		{
			_pointer = pointer;
		}
		
		/*internal function allotPtr():void
		{
			Library.memory.position = _pointer;
			
			xPointer = Library.memory.position;
			
			Library.memory.readFloat();
			
			yPointer = Library.memory.position;
			
			Library.memory.readFloat();
			
			zPointer = Library.memory.position;
		}*/
		
		public override function toString():String
		{
			return "x:" + x + ", y:" + y + ", z:" + z;
		}
	}
}