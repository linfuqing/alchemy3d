package AlchemyLib.geom
{	
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	
	import flash.utils.ByteArray;
	
	public class Vertex3D extends Pointer
	{
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
		
		public function Vertex3D(x:Number = 0, y:Number = 0, z:Number = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}
		
		static public function serialize(input:Vertex3D, data:ByteArray):void
		{
			data.writeFloat(input.x);
			data.writeFloat(input.y);
			data.writeFloat(input.z);
		}
		
		static public function unserialize(data:ByteArray):Vertex3D
		{
			var x:Number;
			var z:Number;
			var y:Number;
			
			x = data.readFloat();
			y = data.readFloat();
			z = data.readFloat();
			
			return new Vertex3D(x, y, z);
		}
		
		internal function setPointer( pointer:uint ):void
		{
			_pointer = pointer;
		}

		public override function toString():String
		{
			return "x:" + x + ", y:" + y + ", z:" + z;
		}
		
		internal var _x:Number;
		internal var _y:Number;
		internal var _z:Number;
	}
}