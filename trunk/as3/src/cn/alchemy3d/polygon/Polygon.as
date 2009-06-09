package cn.alchemy3d.polygon
{

	import cn.alchemy3d.scene.Scene3D;
	
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	
	public class Polygon
	{
		public function Polygon(name:String = "")
		{
			this.ID = this.parentID = this.rootID = -1;
			this.name = name;
			this.visible = true;
			this.childrenNum = 0;
			this.children = new Vector.<Polygon>();
			
			this.direction = new Vector3D();
			this.position = new Vector3D();
			this.scale = new Vector3D();
			
			directionDirty = positionDirty = scaleDirty = headerDirty = transformDirty = true;
		}
		
		public var ID:int;
		public var name:String;
		public var parent:Polygon;
		public var parentID:int;
		public var root:Polygon;
		public var rootID:int;
		public var scene:Scene3D;
		public var visible:Boolean;
		
		protected var _direction:Vector3D;
		protected var _position:Vector3D;
		protected var _scale:Vector3D;
		
		protected var children:Vector.<Polygon>;
		protected var childrenNum:int;
		protected var directionDirty:Boolean;
		protected var headerDirty:Boolean;
		protected var positionDirty:Boolean;
		protected var scaleDirty:Boolean;
		protected var transformDirty:Boolean;
		
		public function get direction():Vector3D
		{
			return _direction;
		}
		
		public function set direction(direction:Vector3D):void
		{
			_direction = direction;
			directionDirty = true;
		}
		
		public function get position():Vector3D
		{
			return _direction;
		}
		
		public function set position(position:Vector3D):void
		{
			_position = position;
			positionDirty = true;
		}
		
		public function get scale():Vector3D
		{
			return _scale;
		}
		
		public function set scale(scale:Vector3D):void
		{
			_scale = scale;
			scaleDirty = true;
		}
		
		public function addChild(child:Polygon):void
		{
			child.parent = this;
			child.root = root;
			child.scene = scene;
			
			children.push(child);
			childrenNum ++;
			
			scene.addChild(child);
		}
		
		/**
		 * 获得该几何体的副本
		 * 
		 * @return 返回复制的几何体
		 */
		public function clone():Polygon
		{
			var object:Polygon = new Polygon();
			
			object._direction = _direction.clone();
			object._position = _position.clone();
			object._scale = _scale.clone();
			
			var child:Polygon;
			for each(child in this.children)
				object.addChild(child.clone());
				
			return object;
		}
		
		/**
		 * 销毁
		 */
		public function destory():void
		{
		}
		
		/**
		 * 序列化
		 */
		public function serialize(buffer:ByteArray, offset:int = 0):void
		{
			if (headerDirty)
			{
				buffer.position = offset;
				
				buffer.writeDouble(ID);			//0
				buffer.writeDouble(parentID);	//1
				buffer.writeDouble(rootID);		//2
				buffer.writeDouble(0);			//3
				buffer.writeDouble(0);			//4
				buffer.writeDouble(0);			//5
				buffer.writeDouble(0);			//6
				buffer.writeDouble(0);			//7
				buffer.writeDouble(0);			//8
				buffer.writeDouble(0);			//9
				
				headerDirty = false;
			}
			
			buffer.writeDouble(0);			//10
			buffer.writeDouble(0);			//11
			buffer.writeDouble(0);			//12
			buffer.writeDouble(0);			//13
			buffer.writeDouble(0);			//14
			buffer.writeDouble(0);			//15
			buffer.writeDouble(0);			//16
			buffer.writeDouble(0);			//17
			buffer.writeDouble(0);			//18
			buffer.writeDouble(0);			//19
			
			if (positionDirty)
			{
				buffer.position = 160 + offset;
				buffer.writeDouble(_position.x);
				buffer.writeDouble(_position.y);
				buffer.writeDouble(_position.z);
				
				positionDirty = false;
			}
			
			if (directionDirty)
			{
				buffer.position = 184 + offset;
				buffer.writeDouble(_direction.x);
				buffer.writeDouble(_direction.y);
				buffer.writeDouble(_direction.z);
				
				directionDirty = false;
			}
			
			if (scaleDirty)
			{
				buffer.position = 208 + offset;
				buffer.writeDouble(_scale.x);
				buffer.writeDouble(_scale.y);
				buffer.writeDouble(_scale.z);
				
				scaleDirty = false;
			}
			
			if (transformDirty)
			{
				buffer.position = 232 + offset;
				
				buffer.writeDouble(1);
				buffer.writeDouble(0);
				buffer.writeDouble(0);
				buffer.writeDouble(0);
				
				buffer.writeDouble(0);
				buffer.writeDouble(1);
				buffer.writeDouble(0);
				buffer.writeDouble(0);
				
				buffer.writeDouble(0);
				buffer.writeDouble(0);
				buffer.writeDouble(1);
				buffer.writeDouble(0);
				
				buffer.writeDouble(0);
				buffer.writeDouble(0);
				buffer.writeDouble(0);
				buffer.writeDouble(1);
				
				transformDirty = false;
			}
		}
	}
}