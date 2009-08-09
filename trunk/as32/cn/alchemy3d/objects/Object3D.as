package cn.alchemy3d.objects
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.geom.Position;
	
	import flash.geom.Vector3D;

	public class Object3D extends Position
	{
		private const MOVE_TYPE_TRANSFORM  :int = 1;
		private const MOVE_TYPE_TRANSLATION:int = 2;
		
		private var positionPointer:uint;
		
		private var scale:Vector3D;
		
		private var scalePointer :uint;
		private var scaleXPointer:uint;
		private var scaleYPointer:uint;
		private var scaleZPointer:uint;
		
		private var rotation:Vector3D;
		
		private var rotationPointer :uint;
		private var rotationXPointer:uint;
		private var rotationYPointer:uint;
		private var rotationZPointer:uint;
		
		private var _target:Position;
		
		private var targetPointer:uint;
		
		private var _interpolation:Number;
		
		private var interpolationPointer:uint;
		
		protected var movePointer:uint;
		
		public function set target( value:Position ):void
		{
			Library.instance().buffer.position = targetPointer;
			
			Library.instance().buffer.writeUnsignedInt( value ? value.vectorPointer : NULL );
		}
		
		public function get target():Position
		{
			return _target;
		}
		
				
		public function set interpolation( value:Number ):void
		{
			value = Math.max( 0, Math.min( 1, value ) );
			
			Library.instance().buffer.position = targetPointer;
			
			Library.instance().buffer.writeDouble( value );
		}
		
		public function get interpolation():Number
		{
			return _interpolation;
		}
		
		override public function get vectorPointer():uint
		{
			return positionPointer;
		}
		
		override public function set x( value:Number ):void
		{
			Library.instance().buffer.position = xPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			if( Library.instance().buffer.readInt() != MOVE_TYPE_TRANSFORM )
			{
				Library.instance().buffer.position = movePointer;
				
				Library.instance().buffer.writeInt( MOVE_TYPE_TRANSLATION );
			}
		}
		
		override public function set y( value:Number ):void
		{
			Library.instance().buffer.position = yPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			if( Library.instance().buffer.readInt() != MOVE_TYPE_TRANSFORM )
			{
				Library.instance().buffer.position = movePointer;
				
				Library.instance().buffer.writeInt( MOVE_TYPE_TRANSLATION );
			}
		}
		
		override public function set z( value:Number ):void
		{
			Library.instance().buffer.position = zPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			if( Library.instance().buffer.readInt() != MOVE_TYPE_TRANSFORM )
			{
				Library.instance().buffer.position = movePointer;
				
				Library.instance().buffer.writeInt( MOVE_TYPE_TRANSLATION );
			}
		}
		
		public function set scaleX( value:Number ):void
		{
			Library.instance().buffer.position = scaleXPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( MOVE_TYPE_TRANSFORM );
		}
		
		public function get scaleX():Number
		{
			return scale.x;
		}
		
		public function set scaleY( value:Number ):void
		{
			Library.instance().buffer.position = scaleYPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( MOVE_TYPE_TRANSFORM );
		}
		
		public function get scaleY():Number
		{
			return scale.y;
		}
		
		public function set scaleZ( value:Number ):void
		{
			Library.instance().buffer.position = scaleZPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( MOVE_TYPE_TRANSFORM );
		}
		
		public function get scaleZ():Number
		{
			return scale.z;
		}
		
		public function set rotationX( value:Number ):void
		{
			Library.instance().buffer.position = rotationXPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( MOVE_TYPE_TRANSFORM );
		}
		
		public function get rotationX():Number
		{
			return rotation.x;
		}
		
		public function set rotationY( value:Number ):void
		{
			Library.instance().buffer.position = rotationYPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( MOVE_TYPE_TRANSFORM );
		}
		
		public function get rotationY():Number
		{
			return rotation.y;
		}
		
		public function set rotationZ( value:Number ):void
		{
			Library.instance().buffer.position = rotationZPointer;
			
			Library.instance().buffer.writeDouble( value );
			
			Library.instance().buffer.position = movePointer;
			
			Library.instance().buffer.writeInt( MOVE_TYPE_TRANSFORM );
		}
		
		public function get rotationZ():Number
		{
			return rotation.z;
		}
		
		public function Object3D( target:Position = null )
		{
			_target  = target;
			
			scale    = new Vector3D();
			
			rotation = new Vector3D();
			
			super();
		}
		
		override protected function initialize():void
		{
			var object:Array    = Library.instance().methods.initializeObject( _target ? _target.vectorPointer : NULL );
			
			_pointer            = object[0];
			
			targetPointer       = object[1];
			
			interplationPointer = object[2];
			
			movePointer         = object[3];
			
			positionPointer     = object[4];
			
			xPointer            = object[5];
			
			yPointer            = object[6];
			
			zPointer            = object[7];
			
			rotationPointer     = object[8];
			
			rotationXPointer    = object[9];
			
			rotationYPointer    = object[10];
			
			rotationZPointer    = object[11];
			
			scalePointer        = object[12];
			
			scaleXPointer       = object[13];
			
			scaleYPointer       = object[14];
			
			scaleZPointer       = object[15];
		}
		
	}
}