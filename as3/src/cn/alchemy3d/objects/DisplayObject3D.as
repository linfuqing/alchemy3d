package cn.alchemy3d.objects
{

	import cn.alchemy3d.scene.Scene3D;
	
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class DisplayObject3D
	{
		public function DisplayObject3D(name:String = "")
		{
			this.ID = this.parentID = this.rootID = -1;
			this.name = name;
			this.visible = true;
			this.childrenNum = 0;
			this.children = new Vector.<DisplayObject3D>();
			
			this._direction = new Vector3D();
			this._position = new Vector3D();
			this._scale = new Vector3D();
			
			directionDirty = positionDirty = scaleDirty = headerDirty = transformDirty = true;
		}
		
		public var ID:int;
		public var name:String;
		public var parent:DisplayObject3D;
		public var parentID:int;
		public var root:DisplayObject3D;
		public var rootID:int;
		public var scene:Scene3D;
		public var visible:Boolean;
		
		protected var _direction:Vector3D;
		protected var _position:Vector3D;
		protected var _scale:Vector3D;
		
		protected var children:Vector.<DisplayObject3D>;
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
		
		public function get scale():Number
		{
			if( _scale.x == _scale.y && _scale.x == _scale.z )
				return _scale.x;
			else
				return NaN;
		}
		
		public function set scale(value:Number):void
		{
			_scale.x = _scale.y = _scale.z;
			scaleDirty = true;
		}
		
		public function get x():Number
		{
			return _position.x;
		}
		
		public function set x(value:Number):void
		{
			_position.x = value;
			positionDirty = true;
		}
		
		public function get y():Number
		{
			return _position.y;
		}
		
		public function set y(value:Number):void
		{
			_position.y = value;
			positionDirty = true;
		}
		
		public function get z():Number
		{
			return _position.z;
		}
		
		public function set z(value:Number):void
		{
			_position.z = value;
			positionDirty = true;
		}
		
		public function get scaleX():Number
		{
			return _scale.x;
		}
		
		public function set scaleX(value:Number):void
		{
			_scale.x = value;
			scaleDirty = true;
		}
		
		public function get scaleY():Number
		{
			return _scale.y;
		}
		
		public function set scaleY(value:Number):void
		{
			_scale.y = value;
			scaleDirty = true;
		}
		
		public function get scaleZ():Number
		{
			return _scale.z;
		}
		
		public function set scaleZ(value:Number):void
		{
			_scale.z = value;
			scaleDirty = true;
		}
		
		public function addChild(child:DisplayObject3D):void
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
		public function clone():DisplayObject3D
		{
			var object:DisplayObject3D = new DisplayObject3D();
			
			object._direction = _direction.clone();
			object._position = _position.clone();
			object._scale = _scale.clone();
			
			var child:DisplayObject3D;
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
		public function serialize():ByteArray
		{
			var buffer:ByteArray = new ByteArray();
			buffer.endian = Endian.LITTLE_ENDIAN;
			
			if (headerDirty)
			{
				buffer.position = 0;
				
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
				buffer.position = 160;
				buffer.writeDouble(_position.x);	//20
				buffer.writeDouble(_position.y);	//21
				buffer.writeDouble(_position.z);	//22
				
				positionDirty = false;
			}
			
			if (directionDirty)
			{
				buffer.position = 184;
				buffer.writeDouble(_direction.x);	//23
				buffer.writeDouble(_direction.y);	//24
				buffer.writeDouble(_direction.z);	//25
				
				directionDirty = false;
			}
			
			if (scaleDirty)
			{
				buffer.position = 208;
				buffer.writeDouble(_scale.x);	//26
				buffer.writeDouble(_scale.y);	//27
				buffer.writeDouble(_scale.z);	//28
				
				scaleDirty = false;
			}
			
			if (transformDirty)
			{
				buffer.position = 232;
				
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
			
			return buffer;
		}
	}
}