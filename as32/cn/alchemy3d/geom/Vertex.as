package cn.alchemy3d.geom
{	
	import cn.alchemy3d.base.Instance;
	
	public class Vertex extends Instance
	{
		private var _x:Number;
		private var _y:Number;
		private var _z:Number;
		
		internal var xPointer:uint;
		internal var yPointer:uint;
		internal var zPointer:uint;
		
		internal var worldXPointer:uint;
		internal var worldYPointer:uint;
		internal var worldZPointer:uint;
		
		public function set x( value:Number ):void
		{
			_x = value;
			
			if( pointer )
			{
				instance.buffer.position = xPointer;
				
				instance.buffer.writeDouble( _x );
			}
		}
		
		public function get x():Number
		{
			return _x;
		}
		
		public function set y( value:Number ):void
		{
			_y = value;
			
			if( pointer )
			{
				instance.buffer.position = yPointer;
				
				instance.buffer.writeDouble( _y );
			}
		}
		
		public function get y():Number
		{
			return _y;
		}
		
		public function set z( value:Number ):void
		{
			_z = value;
			
			if( pointer )
			{
				instance.buffer.position = zPointer;
				
				instance.buffer.writeDouble( _z );
			}
		}
		
		public function get z():Number
		{
			return _z;
		}
		
		public function get worldX():Number
		{
			if( pointer )
			{
				instance.buffer.position = worldXPointer;
				
				return instance.buffer.readDouble();
			}
			
			return 0;
		}
		
		public function get worldY():Number
		{
			if( pointer )
			{
				instance.buffer.position = worldYPointer;
				
				return instance.buffer.readDouble();
			}
			
			return 0;
		}
		
		public function get worldZ():Number
		{
			if( pointer )
			{
				instance.buffer.position = worldZPointer;
				
				return instance.buffer.readDouble();
			}
			
			return 0;
		}
		
		public function Vertex( x:Number = 0, y:Number = 0, z:Number = 0 )
		{
			super();
			
			_x = x;
			_y = y;
			_z = z;
		}
		
		public function toString():String
		{
			return "x:" + _x + ", y:" + _y + ", z:" + _z;
		}
	}
}