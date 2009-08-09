package cn.alchemy3d.geom
{	
	import cn.alchemy3d.base.Library;
	
	public class Vertex extends Position
	{
		private var positionPointer:uint;
		
		private var worldPointer:uint;
		
		private var worldXPointer:uint;
		private var worldYPointer:uint;
		private var worldZPointer:uint;
		
		public override function get vectorPosition():uint
		{
			return worldPointer;
		}
		
		override public function set x( value:Number ):void
		{
			position.x = value;
			
			if( _pointer )
			{
				if( xPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = xPointer;
				
				Library.instance().buffer.writeDouble( position.x );
			}
		}
		
		override public function set y( value:Number ):void
		{
			position.y = value;
			
			if( _pointer )
			{
				if( yPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = yPointer;
				
				Library.instance().buffer.writeDouble( position.y );
			}
		}
		
		public function set z( value:Number ):void
		{
			position.z = value;
			
			if( _pointer )
			{
				if( zPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = zPointer;
				
				Library.instance().buffer.writeDouble( position.z );
			}
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
			super( x, y, z );
			
			init();
		}
		
		private function init():void
		{
			positionPointer = NULL;
			
			xPointer        = NULL;
			yPointer        = NULL;
			zPointer        = NULL;
			
			worldPointer    = NULL;
			
			worldXPointer   = NULL;
			worldYPointer   = NULL;
			worldZPointer   = NULL;
		}
		
		override protected function initialize():void
		{
			
		}
		
		private function setPointers():void
		{
			if( _pointer )
			{
				var vertex:Array = instance.library.initializeVertex( v.pointer );
				
				positionPointer  = vertex[0];
				
				xPointer         = vertex[1];
				yPointer         = vertex[2];
				zPointer         = vertex[3];
				
				worldPointer     = vertex[4];
				
				worldXPointer    = vertex[5];
				worldYPointer    = vertex[6];
				worldZPointer    = vertex[7];
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