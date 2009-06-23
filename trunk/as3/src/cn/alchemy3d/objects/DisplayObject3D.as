package cn.alchemy3d.objects
{

	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	
	public class DisplayObject3D
	{
		public function DisplayObject3D(name:String)
		{
			this.name = name;
			this.visible = true;
			
			this._direction = new Vector3D();
			this._position = new Vector3D();
			this._scale = new Vector3D();
			
			var lib:Alchemy3DLib = Alchemy3DLib.getInstance();
			
			//初始化场景
			buffer = lib.buffer;
		}
		
		public var pointer:uint;
		public var directionPtr:uint;
		public var positionPtr:uint;
		public var scalePtr:uint;
		
		public var name:String;
		public var parent:DisplayObject3D;
		public var root:DisplayObject3D;
		public var scene:Scene3D;
		public var visible:Boolean;
		
		protected var _direction:Vector3D;
		protected var _position:Vector3D;
		protected var _scale:Vector3D;
		
		protected var buffer:ByteArray;
		
		public function get direction():Vector3D
		{
			return _direction;
		}
		
		public function set direction(direction:Vector3D):void
		{
			_direction = direction;
			buffer.position = directionPtr;
			buffer.writeDouble(direction.x);
			buffer.writeDouble(direction.y);
			buffer.writeDouble(direction.z);
		}
		
		public function get position():Vector3D
		{
			return _direction;
		}
		
		public function set position(position:Vector3D):void
		{
			_position = position;
			buffer.position = positionPtr;
			buffer.writeDouble(direction.x);
			buffer.writeDouble(direction.y);
			buffer.writeDouble(direction.z);
		}
		
		public function get scale():Number
		{
			if( _scale.x == _scale.y && _scale.x == _scale.z )
				return _scale.x;
			else
				return NaN;
		}
		
		public function set scale(value:Number):void
		{
			_scale.x = _scale.y = _scale.z = value;
			buffer.position = scalePtr;
			buffer.writeDouble(value);
			buffer.writeDouble(value);
			buffer.writeDouble(value);
		}
		
		public function get x():Number
		{
			return _position.x;
		}
		
		public function set x(value:Number):void
		{
			_position.x = value;
			buffer.position = positionPtr;
			buffer.writeDouble(value);
		}
		
		public function get y():Number
		{
			return _position.y;
		}
		
		public function set y(value:Number):void
		{
			_position.y = value;
			buffer.position = positionPtr + 8;
			buffer.writeDouble(value);
		}
		
		public function get z():Number
		{
			return _position.z;
		}
		
		public function set z(value:Number):void
		{
			_position.z = value;
			buffer.position = positionPtr + 16;
			buffer.writeDouble(value);
		}
		
		public function get scaleX():Number
		{
			return _scale.x;
		}
		
		public function set scaleX(value:Number):void
		{
			_scale.x = value;
			buffer.position = scalePtr;
			buffer.writeDouble(value);
		}
		
		public function get scaleY():Number
		{
			return _scale.y;
		}
		
		public function set scaleY(value:Number):void
		{
			_scale.y = value;
			buffer.position = scalePtr + 8;
			buffer.writeDouble(value);
		}
		
		public function get scaleZ():Number
		{
			return _scale.z;
		}
		
		public function set scaleZ(value:Number):void
		{
			_scale.z = value;
			buffer.position = scalePtr + 16;
			buffer.writeDouble(value);
		}
		
		public function set rotationX(value:Number):void
		{
			_direction.x = value;
			buffer.position = directionPtr;
			buffer.writeDouble(direction.x);
		}
		
		public function get rotationX():Number
		{
			return _direction.x;
		}
		
		public function set rotationY(value:Number):void
		{
			_direction.y = value;
			buffer.position = directionPtr + 8;
			buffer.writeDouble(direction.y);
		}
		
		public function get rotationY():Number
		{
			return _direction.y;
		}
		
		public function set rotationZ(value:Number):void
		{
			_direction.z = value;
			buffer.position = directionPtr + 16;
			buffer.writeDouble(direction.z);
		}
		
		public function get rotationZ():Number
		{
			return _direction.z;
		}
		
		/**
		 * 获得该几何体的副本
		 * 
		 * @return 返回复制的几何体
		 */
		public function clone():DisplayObject3D
		{
			return null;
		}
	}
}