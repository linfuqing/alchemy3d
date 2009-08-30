package cn.alchemy3d.objects
{

	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.events.EventDispatcher;
	import flash.geom.Vector3D;
	
	public class Entity extends EventDispatcher implements ISceneNode
	{
		public function Entity(material:Material = null, texture:Texture = null, name:String = "")
		{
			this.name = name;
			this.visible = true;
			
			this._material = material;
			this._texture = texture;
			
			this._direction = new Vector3D();
			this._position = new Vector3D();
			this._scale = new Vector3D();
			this._worldPosition = new Vector3D();
			
			this._children = new Vector.<Entity>();
			this._root = this;
			this._lightEnable = false;
		}
		
		public var pointer:uint;
		
		public var directionPtr:uint;
		public var positionPtr:uint;
		public var scalePtr:uint;
		public var worldPositionPtr:uint;
		public var materialPtr:int;
		public var texturePtr:int;
		public var lightEnablePtr:uint;
		
		public var name:String;
		public var visible:Boolean;
		
		private var _direction:Vector3D;
		private var _position:Vector3D;
		private var _scale:Vector3D;
		private var _worldPosition:Vector3D;
		
		private var _material:Material;
		private var _texture:Texture;
		
		private var _parent:Entity;
		private var _children:Vector.<Entity>;
		private var _root:Entity;
		private var _scene:Scene3D;
		private var _lightEnable:Boolean;
		
		protected static const sizeOfInt:int = 4;
		
		public function get parent():Entity
		{
			return this._parent;
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
		
		public function get lightEnable():Boolean
		{
			return _lightEnable;
		}
		
		public function set lightEnable(bool:Boolean):void
		{
			if (!checkInitialized()) return;
			
			_lightEnable = bool;
			
			Library.memory.position = lightEnablePtr;
			
			if (bool)
				Library.memory.writeUnsignedInt(1);
			else
				Library.memory.writeUnsignedInt(0);
		}
		
		public function get material():Material
		{
			return _material;
		}
		
		public function set material(material:Material):void
		{
			if (!checkInitialized()) return;
			
			Library.memory.position = materialPtr;
			Library.memory.writeUnsignedInt(material.pointer);
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function set texture(texture:Texture):void
		{
			if (!checkInitialized()) return;
			
			Library.memory.position = texturePtr;
			
			if (texture)
				Library.memory.writeUnsignedInt(texture.pointer);
			else
				Library.memory.writeUnsignedInt(0);
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
		
		public function addEntity(child:Entity):void
		{
			if (child._parent || child._scene) throw new Error("已存在父节点");
			
			this._children.push(child);
			child._parent = this;
			child._root = this;
			
			if (this.scene)
			{
				child.initialize(this.scene);
				
				if (child is Mesh3D)
				{
					this.scene.verticesNum += Mesh3D(child).nVertices;
					this.scene.facesNum += Mesh3D(child).nFaces;
				}
			}
		}
		
		public function initialize(scene:Scene3D):void
		{
			this._scene = scene;
			
			allotPtr(callAlchemy());
			
			for each (var child:Entity in this._children)
			{
				child.initialize(scene);
			}
		}
		
		protected function callAlchemy():Array
		{
			var tPtr:uint = texture == null ? 0 : texture.pointer;
			var mPtr:uint = material == null ? 0 : material.pointer;
			var parentPtr:uint = _parent == null ? 0 : _parent.pointer;
			var scenePtr:uint = scene == null ? 0 : scene.pointer;
			
			return Library.alchemy3DLib.initializeEntity(scenePtr, parentPtr, mPtr, tPtr, 0, 0, 0);
		}
		
		protected function allotPtr(ps:Array):void
		{
			pointer				= ps[0];
			positionPtr			= ps[1];
			directionPtr		= ps[2];
			scalePtr			= ps[3];
			worldPositionPtr	= ps[4];
			lightEnablePtr		= ps[5];
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