package cn.alchemy3d.geom
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;

	public class UV extends Pointer
	{
		private var _u:Number;
		private var _v:Number;
		
		internal var uPointer:Number;
		internal var vPointer:Number;
		
		public function set u( value:Number ):void
		{
			_u = value;
			
			if( pointer )
			{
				if( uPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = uPointer;
				
				Library.instance().buffer.writeDouble( _u );
			}
		}
		
		public function get u():Number
		{
			return _u;
		}
		
		public function set v( value:Number ):void
		{
			_v = value;
			
			if( pointer )
			{
				if( vPointer == NULL )
				{
					setPointers();
				}
				
				Library.instance().buffer.position = vPointer;
				
				Library.instance().buffer.writeDouble( _v );
			}
		}
		
		public function get v():Number
		{
			return _v;
		}
		
		public function UV( u:Number, v:Number )
		{
			super();
			
			_u = u;
			_v = v;
			
			init();
		}
		
		private function init():void
		{
			uPointer = NULL;
			vPointer = NULL;
		}
		
		private function setPointers():void
		{
			var uv:Array = Library.instance().methods.initializeVector( f.uvs[i].pointer );
					
			uPointer     = uv[0];
			vPointer     = uv[1];
		}
		
		internal function setPointer( value:uint ):void
		{
			_pointer = value;
		}
	}
}