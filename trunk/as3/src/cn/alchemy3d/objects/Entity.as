package cn.alchemy3d.objects
{

	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.events.EventDispatcher;
	import flash.geom.Vector3D;
	
	public class Entity extends EventDispatcher
	{
		public function Entity(name:String = "", mesh:Mesh3D = null)
		{
			this.name = name;
			this.visible = true;
			
			this._mesh = mesh;
			
			this._direction = new Vector3D();
			this._position = new Vector3D();
			this._scale = new Vector3D();
			this._worldPosition = new Vector3D();
			
			this._children = new Vector.<Entity>();
			this._root = this;
			
			initialize();
		}
		
		public var pointer:uint;
		
		public var directionPtr:uint;
		public var positionPtr:uint;
		public var scalePtr:uint;
		public var worldPositionPtr:uint;
		public var meshPtr:uint;
		
		public var name:String;
		public var visible:Boolean;
		
		private var _direction:Vector3D;
		private var _position:Vector3D;
		private var _scale:Vector3D;
		private var _worldPosition:Vector3D;
		
		private var _mesh:Mesh3D;
		private var _parent:Entity;
		private var _children:Vector.<Entity>;
		private var _root:Entity;
		private var _scene:Scene3D;
		
		protected static const sizeOfInt:int = 4;
		
		public function get parent():Entity
		{
			return this._parent;
		}
		
		public function set parent(parent:Entity):void
		{
			this._parent = parent;
		}
		
		public function get children():Vector.<Entity>
		{
			return this._children;
		}
		
		public function get root():Entity
		{
			return this._root;
		}
		
		public function get scene():Scene3D
		{
			return this._scene;
		}
		
		public function get mesh():Mesh3D
		{
			return _mesh;
		}
		
		public function set mesh(mesh:Mesh3D):void
		{
			if(!checkInitialized()) return;
			
			_mesh = mesh;
			
			Library.memory.position = meshPtr;
			Library.memory.writeUnsignedInt(mesh.pointer);
		}
		
		public function get direction():Vector3D
		{
			return _direction;
		}
		
		public function set direction(direction:Vector3D):void
		{
			if (!checkInitialized()) return;
			
			_direction = direction;
			
			Library.memory.position = directionPtr;
			Library.memory.writeFloat(direction.x);
			Library.memory.writeFloat(direction.y);
			Library.memory.writeFloat(direction.z);
		}
		
		public function get position():Vector3D
		{
			return _position;
		}
		
		public function set position(position:Vector3D):void
		{
			if (!checkInitialized()) return;
			
			_position = position;
			
			Library.memory.position = positionPtr;
			Library.memory.writeFloat(position.x);
			Library.memory.writeFloat(position.y);
			Library.memory.writeFloat(position.z);
		}
		
		public function get worldPosition():Vector3D
		{
			Library.memory.position = worldPositionPtr;
			_worldPosition.x = Library.memory.readFloat();
			_worldPosition.y = Library.memory.readFloat();
			_worldPosition.z = Library.memory.readFloat();
			
			return _worldPosition;
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
			Library.memory.position = scalePtr;
			Library.memory.writeFloat(value);
			Library.memory.writeFloat(value);
			Library.memory.writeFloat(value);
		}
		
		public function get x():Number
		{
			return _position.x;
		}
		
		public function set x(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_position.x = value;
			Library.memory.position = positionPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get y():Number
		{
			return _position.y;
		}
		
		public function set y(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_position.y = value;
			Library.memory.position = positionPtr + sizeOfInt;
			Library.memory.writeFloat(value);
		}
		
		public function get z():Number
		{
			return _position.z;
		}
		
		public function set z(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_position.z = value;
			Library.memory.position = positionPtr + sizeOfInt * 2;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleX():Number
		{
			return _scale.x;
		}
		
		public function set scaleX(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_scale.x = value;
			Library.memory.position = scalePtr;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleY():Number
		{
			return _scale.y;
		}
		
		public function set scaleY(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_scale.y = value;
			Library.memory.position = scalePtr + sizeOfInt;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleZ():Number
		{
			return _scale.z;
		}
		
		public function set scaleZ(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_scale.z = value;
			Library.memory.position = scalePtr + sizeOfInt * 2;
			Library.memory.writeFloat(value);
		}
		
		public function set rotationX(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_direction.x = value;
			Library.memory.position = directionPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationX():Number
		{
			return _direction.x;
		}
		
		public function set rotationY(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_direction.y = value;
			Library.memory.position = directionPtr + sizeOfInt;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationY():Number
		{
			return _direction.y;
		}
		
		public function set rotationZ(value:Number):void
		{
			if (!checkInitialized()) return;
			
			_direction.z = value;
			Library.memory.position = directionPtr + sizeOfInt * 2;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationZ():Number
		{
			return _direction.z;
		}
		
		protected function initialize():void
		{
			allotPtr(callAlchemy());
		}
		
		protected function callAlchemy():Array
		{
			return Library.alchemy3DLib.initializeEntity(name,_mesh ? _mesh.pointer : 0);
		}
		
		protected function allotPtr(ps:Array):void
		{
			pointer				= ps[0];
			positionPtr			= ps[1];
			directionPtr		= ps[2];
			scalePtr			= ps[3];
			worldPositionPtr	= ps[4];
			meshPtr             = ps[5];
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
				Alchemy3DLog.warning(this.name + "在设置实体属性前必须先初始化！");
				return false;
			}
			
			return true;
		}
	}
}