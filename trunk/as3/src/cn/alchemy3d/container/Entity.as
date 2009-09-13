// A OK version

package cn.alchemy3d.container
{

	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.lights.Light3D;
	
	import flash.geom.Vector3D;
	
	public class Entity extends SceneContainer
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
			
			//this._children = new Vector.<Entity>();
			//this._root = this;
			
			//children = new Vector.<Scene3D>();
			//lights   = new Vector.<Light3D>();
			
			super();
		}
		
		//public var pointer:uint;
		
		//private var lightsPtr:uint = NULL;
		
		private var directionPtr:uint;
		private var positionPtr:uint;
		private var scalePtr:uint;
		private var worldPositionPtr:uint;
		private var meshPtr:uint;
		
		public var name:String;
		public var visible:Boolean;
		
		private var _direction:Vector3D;
		private var _position:Vector3D;
		private var _scale:Vector3D;
		private var _worldPosition:Vector3D;
		
		private var _mesh:Mesh3D;
		private var _parent:Entity;
		//private var _children:Vector.<Entity>;
		//private var _root:Scene3D;
		//private var _scene:Scene3D;
		
		//protected static const sizeOfInt:int = 4;
		
		public function get parent():Entity
		{
			return this._parent;
		}
		
		public function set parent(parent:Entity):void
		{
			this._parent = parent;
		}
		
		/*public function get children():Vector.<Entity>
		{
			return this._children;
		}*/
		
		/*public function get root():Scene3D
		{
			return this._root;
		}*/
		
		/*public function get scene():Scene3D
		{
			return this._scene;
		}*/
		
		public function get mesh():Mesh3D
		{
			return _mesh;
		}
		
		public function set mesh(mesh:Mesh3D):void
		{
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
			_position.y = value;
			Library.memory.position = positionPtr + Library.intTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get z():Number
		{
			return _position.z;
		}
		
		public function set z(value:Number):void
		{
			_position.z = value;
			Library.memory.position = positionPtr + Library.intTypeSize * 2;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleX():Number
		{
			return _scale.x;
		}
		
		public function set scaleX(value:Number):void
		{
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
			_scale.y = value;
			Library.memory.position = scalePtr + Library.intTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleZ():Number
		{
			return _scale.z;
		}
		
		public function set scaleZ(value:Number):void
		{
			_scale.z = value;
			Library.memory.position = scalePtr + Library.intTypeSize * 2;
			Library.memory.writeFloat(value);
		}
		
		public function set rotationX(value:Number):void
		{
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
			_direction.y = value;
			Library.memory.position = directionPtr + Library.intTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationY():Number
		{
			return _direction.y;
		}
		
		public function set rotationZ(value:Number):void
		{
			_direction.z = value;
			Library.memory.position = directionPtr + Library.intTypeSize * 2;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationZ():Number
		{
			return _direction.z;
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeEntity(name,_mesh ? _mesh.pointer : NULL);

			_pointer		    = ps[0];
			positionPtr			= ps[1];
			directionPtr		= ps[2];
			scalePtr			= ps[3];
			worldPositionPtr	= ps[4];
			meshPtr             = ps[5];
			_node               = ps[6];
		}
	}
}