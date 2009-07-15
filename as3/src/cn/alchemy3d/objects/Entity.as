package cn.alchemy3d.objects
{

	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	
	public class Entity implements ISceneNode
	{
		public function Entity(material:Material = null, name:String = "")
		{
			this.name = name;
			this.visible = true;
			
			this._material = material;
			
			this._direction = new Vector3D();
			this._position = new Vector3D();
			this._scale = new Vector3D();
			
			lib = Alchemy3DLib.getInstance();
			
			buffer = lib.buffer;
		}
		
		public var pointer:uint;
		protected var lib:Alchemy3DLib;	
		protected var buffer:ByteArray;
		
		public var directionPtr:uint;
		public var positionPtr:uint;
		public var scalePtr:uint;
		
		public var name:String;
		public var parent:Entity;
		public var root:Entity;
		public var scene:Scene3D;
		public var visible:Boolean;
		
		private var _direction:Vector3D;
		private var _position:Vector3D;
		private var _scale:Vector3D;
		
		public var materialPtr:int;
		private var _material:Material;
		
		public var texturePtr:int;
		
		protected static const sizeOfType:int = 4;
		
		public function get material():Material
		{
			return _material;
		}
		
		public function set material(material:Material):void
		{
			if (!checkInitialized()) return;
			
			buffer.position = materialPtr;
			buffer.writeUnsignedInt(material.pointer);
		}
		
		public function get direction():Vector3D
		{
			return _direction;
		}
		
		public function set direction(direction:Vector3D):void
		{
			if (!checkInitialized()) return;
			
			_direction = direction;
			buffer.position = directionPtr;
			buffer.writeFloat(direction.x);
			buffer.writeFloat(direction.y);
			buffer.writeFloat(direction.z);
		}
		
		public function get position():Vector3D
		{
			return _direction;
		}
		
		public function set position(position:Vector3D):void
		{
			if (!checkInitialized()) return;
			
			_position = position;
			buffer.position = positionPtr;
			buffer.writeFloat(direction.x);
			buffer.writeFloat(direction.y);
			buffer.writeFloat(direction.z);
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
			if (!checkInitialized()) return;
			
			_scale.x = _scale.y = _scale.z = value;
			buffer.position = scalePtr;
			buffer.writeFloat(value);
			buffer.writeFloat(value);
			buffer.writeFloat(value);
		}
		
		public function get x():Number
		{
			return _position.x;
		}
		
		public function set x(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_position.x = value;
			buffer.position = positionPtr;
			buffer.writeFloat(value);
		}
		
		public function get y():Number
		{
			return _position.y;
		}
		
		public function set y(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_position.y = value;
			buffer.position = positionPtr + sizeOfType;
			buffer.writeFloat(value);
		}
		
		public function get z():Number
		{
			return _position.z;
		}
		
		public function set z(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_position.z = value;
			buffer.position = positionPtr + sizeOfType * 2;
			buffer.writeFloat(value);
		}
		
		public function get scaleX():Number
		{
			return _scale.x;
		}
		
		public function set scaleX(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_scale.x = value;
			buffer.position = scalePtr;
			buffer.writeFloat(value);
		}
		
		public function get scaleY():Number
		{
			return _scale.y;
		}
		
		public function set scaleY(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_scale.y = value;
			buffer.position = scalePtr + sizeOfType;
			buffer.writeFloat(value);
		}
		
		public function get scaleZ():Number
		{
			return _scale.z;
		}
		
		public function set scaleZ(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_scale.z = value;
			buffer.position = scalePtr + sizeOfType * 2;
			buffer.writeFloat(value);
		}
		
		public function set rotationX(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_direction.x = value;
			buffer.position = directionPtr;
			buffer.writeFloat(value);
		}
		
		public function get rotationX():Number
		{
			return _direction.x;
		}
		
		public function set rotationY(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_direction.y = value;
			buffer.position = directionPtr + sizeOfType;
			buffer.writeFloat(value);
		}
		
		public function get rotationY():Number
		{
			return _direction.y;
		}
		
		public function set rotationZ(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_direction.z = value;
			buffer.position = directionPtr + sizeOfType * 2;
			buffer.writeFloat(value);
		}
		
		public function get rotationZ():Number
		{
			return _direction.z;
		}
		
		public function initialize(scenePtr:uint, parentPtr:uint):void
		{
			allotPtr(lib.alchemy3DLib.initializeEntity(scenePtr, parentPtr, material.pointer, 0, 0, 0, 0));
		}
		
		public function allotPtr(ps:Array):void
		{
			pointer = ps[0];
			materialPtr = ps[1];
			positionPtr = ps[2];
			directionPtr = ps[3];
			scalePtr = ps[4];
			materialPtr = ps[5];
			texturePtr = ps[6];
		}
		
		/**
		 * 获得该几何体的副本
		 * 
		 * @return 返回复制的几何体
		 */
		public function clone():Entity
		{
			return null;
		}
		
		protected function checkInitialized():Boolean
		{
			if (this.pointer == 0)
			{
				Alchemy3DLog.warning("在设置实体属性前必须先初始化！");
				return false;
			}
			
			return true;
		}
	}
}