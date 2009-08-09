package cn.alchemy3d.geom
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	import flash.geom.Vector3D;

	public class Position extends Pointer
	{
		protected var position:Vector3D;
		
		protected var xPointer:uint;
		protected var yPointer:uint;
		protected var zPointer:uint;
		
		public function set x( value:Number ):void
		{
			position.x = value;
			
			Library.instance().buffer.position = xPointer;
			
			Library.instance().buffer.writeDouble( value );
		}
		
		public function get x():Number
		{
			return position.x;
		}
		
		public function set y( value:Number ):void
		{
			position.y = value;
			
			Library.instance().buffer.position = yPointer;
			
			Library.instance().buffer.writeDouble( value );
		}
		
		public function get y():Number
		{
			return position.y;
		}
		
		public function set z( value:Number ):void
		{
			position.z = value;
			
			Library.instance().buffer.position = zPointer;
			
			Library.instance().buffer.writeDouble( value );
		}
		
		public function get z():Number
		{
			return position.z;
		}
		
		public function get vectorPointer():uint
		{
			return _pointer;
		}
		
		public function Position( x:Number = 0, y:Number = 0, z:Number = 0 )
		{
			position = new Vector3D( x, y, z );

			super();
		}
		
		override protected function initialize():void
		{
			var vector:Array = Library.instance().methods.initializeVector3D( position.x, position.y, position.z );
			
			_pointer = vector[0];
			
			xPointer = vector[1];
			
			yPointer = vector[2];
			
			zPointer = vector[3];
		}
		
	}
}