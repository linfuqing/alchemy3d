package cn.alchemy3d.geom
{	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	public class Vertex extends Pointer
	{
		private var _x:Number;
		private var _y:Number;
		private var _z:Number;
		
		private var xPointer:uint;
		private var yPointer:uint;
		private var zPointer:uint;
		
		private var worldXPointer:uint;
		private var worldYPointer:uint;
		private var worldZPointer:uint;
		
		public function set x( value:Number ):void
		{
			_x = value;
			
			if( _pointer )
			{
				if( xPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = xPointer;
				
				Library.instance().buffer.writeDouble( _x );
			}
		}
		
		public function get x():Number
		{
			return _x;
		}
		
		public function set y( value:Number ):void
		{
			_y = value;
			
			if( _pointer )
			{
				if( yPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = yPointer;
				
				Library.instance().buffer.writeDouble( _y );
			}
		}
		
		public function get y():Number
		{
			return _y;
		}
		
		public function set z( value:Number ):void
		{
			_z = value;
			
			if( _pointer )
			{
				if( zPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = zPointer;
				
				Library.instance().buffer.writeDouble( _z );
			}
		}
		
		public function get z():Number
		{
			return _z;
		}
		
		public function get worldX():Number
		{
			if( _pointer )
			{
				if( worldXPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = worldXPointer;
				
				return Library.instance().buffer.readDouble();
			}
			
			return 0;
		}
		
		public function get worldY():Number
		{
			if( _pointer )
			{
				if( worldYPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = worldYPointer;
				
				return Library.instance().buffer.readDouble();
			}
			
			return 0;
		}
		
		public function get worldZ():Number
		{
			if( _pointer )
			{
				if( worldZPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = worldZPointer;
				
				return Library.instance().buffer.readDouble();
			}
			
			return 0;
		}
		
		public function Vertex( x:Number = 0, y:Number = 0, z:Number = 0 )
		{
			super();
			
			_x = x;
			_y = y;
			_z = z;
			
			init();
		}
		
		private function init():void
		{
			xPointer = NULL;
			yPointer = NULL;
			zPointer = NULL;
			
			worldXPointer = NULL;
			worldYPointer = NULL;
			worldZPointer = NULL;
		}
		
		private function setPointers():void
		{
			if( _pointer )
			{
				var vertex:Array = instance.library.initializeVertex( v.pointer );
				
				xPointer         = vertex[0];
				yPointer         = vertex[1];
				zPointer         = vertex[2];
				
				worldXPointer    = vertex[3];
				worldYPointer    = vertex[4];
				worldZPointer    = vertex[5];
			}
		}
		
		internal function setPointer( value:uint ):void
		{
			_pointer = value;
		}
		
		public function toString():String
		{
			return "x:" + _x + ", y:" + _y + ", z:" + _z;
		}
	}
}