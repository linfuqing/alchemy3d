package cn.alchemy3d.geom
{
	import cn.alchemy3d.base.Instance;

	public class UV extends Instance
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
				instance.buffer.position = uPointer;
				
				instance.buffer.writeDouble( _u );
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
				instance.buffer.position = uPointer;
				
				instance.buffer.writeDouble( _v );
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
		}
		
	}
}