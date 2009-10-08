package cn.alchemy3d.view
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	import cn.alchemy3d.container.Entity;
	
	import flash.geom.Vector3D;
	
	public class Camera3D extends Pointer
	{
		//public var pointer:uint = 0;
		
		private var targetPtr:uint;
		private var isUVNPtr:uint;
		private var fovPtr:uint;
		private var nearPtr:uint;
		private var farPtr:uint;
		private var fnfDirtyPtr:int;
		
		private var _target:Vector3D;
		private var _fov:Number;
		private var _near:Number;
		private var _far:Number;
		
		protected var eye:Entity;
		
		public function get x():Number
		{
			return eye.x;
		}
		
		public function set x(value:Number):void
		{
			eye.x = value;
		}
		
		public function get y():Number
		{
			return eye.y;
		}
		
		public function set y(value:Number):void
		{
			eye.y = value;
		}
		
		public function get z():Number
		{
			return eye.z;
		}
		
		public function set z(value:Number):void
		{
			eye.z = value;
		}
		
		public function get rotationX():Number
		{
			return eye.rotationX;
		}
		
		public function set rotationX(value:Number):void
		{
			eye.rotationX = value;
		}
		
		public function get rotationY():Number
		{
			return eye.rotationY;
		}
		
		public function set rotationY(value:Number):void
		{
			eye.rotationY = value;
		}
		
		public function get rotationZ():Number
		{
			return eye.rotationZ;
		}
		
		public function set rotationZ(value:Number):void
		{
			eye.rotationZ = value;
		}
		
		public function get target():Vector3D
		{
			return _target;
		}
		
		public function set target(target:Vector3D):void
		{
			this._target = target;
			
			if (target)
			{
				Library.memory.position = targetPtr;
				Library.memory.writeFloat(target.x);
				Library.memory.writeFloat(target.y);
				Library.memory.writeFloat(target.z);
				
				Library.memory.position = isUVNPtr;
				Library.memory.writeInt(TRUE);
			}
			else
			{
				Library.memory.position = isUVNPtr;
				Library.memory.writeInt(FALSE);
			}
		}
		
		public function get fov():Number
		{
			return _fov;
		}
		
		public function set fov(value:Number):void
		{
			this._fov = value;
			
			Library.memory.position = fovPtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = fnfDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function get far():Number
		{
			return _far;
		}
		
		public function set far(value:Number):void
		{
			this._far = value;
			
			Library.memory.position = farPtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = fnfDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function get near():Number
		{
			return _near;
		}
		
		public function set near(value:Number):void
		{
			this._near = value;
			
			Library.memory.position = nearPtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = fnfDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function Camera3D(fov:Number = 90, near:Number = 20, far:Number = 5000, eye:Entity = null)
		{
			this._fov = fov;
			this._near = near < 20 ? 20 : near;
			this._far = far;
			
			this.eye = eye == null ? new Entity("camera_eye") : eye;
			
			super();
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeCamera(eye.pointer, _fov, _near, _far);

			_pointer = ps[0];
			targetPtr = ps[1];
			fovPtr = ps[2];
			nearPtr = ps[3];
			farPtr = ps[4];
			fnfDirtyPtr = ps[5];
			isUVNPtr = ps[6];
		}
		
		public function hover(mouseX:Number, mouseY:Number, camSpeed:Number):void
		{
			this.eye.x -= (this.eye.x - 1000 * mouseX) / camSpeed;
			this.eye.y -= (this.eye.y - 1000 * mouseY) / camSpeed;
		}
	}
}