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
	import flash.utils.ByteArray;
	
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
			
			lib = Library.getInstance();
			
			buffer = lib.buffer;
		}
		
		public var pointer:uint;
		protected var lib:Library;	
		protected var buffer:ByteArray;
		
		public var directionPtr:uint;
		public var positionPtr:uint;
		public var scalePtr:uint;
		public var worldPositionPtr:uint;
		public var materialPtr:int;
		public var texturePtr:int;
		
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
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function set texture(texture:Texture):void
		{
			if (!checkInitialized()) return;
			
			buffer.position = texturePtr;
			if (texture)
				buffer.writeUnsignedInt(texture.pointer);
			else
				buffer.writeUnsignedInt(0);
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
		
		public function get worldPosition():Vector3D
		{
			buffer.position = worldPositionPtr;
			_worldPosition.x = buffer.readFloat();
			_worldPosition.y = buffer.readFloat();
			_worldPosition.z = buffer.readFloat();
			
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
			buffer.position = positionPtr + sizeOfInt;
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
			buffer.position = positionPtr + sizeOfInt * 2;
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
			buffer.position = scalePtr + sizeOfInt;
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
			buffer.position = scalePtr + sizeOfInt * 2;
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
			buffer.position = directionPtr + sizeOfInt;
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
			buffer.position = directionPtr + sizeOfInt * 2;
			buffer.writeFloat(value);
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
			
			return lib.alchemy3DLib.initializeEntity(scenePtr, parentPtr, mPtr, tPtr, 0, 0, 0);
		}
		
		protected function allotPtr(ps:Array):void
		{
			pointer = ps[0];
			materialPtr = ps[1];
			texturePtr = ps[2];
			positionPtr = ps[3];
			directionPtr = ps[4];
			scalePtr = ps[5];
			worldPositionPtr = ps[6];
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