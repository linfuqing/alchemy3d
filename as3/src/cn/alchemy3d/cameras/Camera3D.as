package cn.alchemy3d.cameras
{
	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.objects.Entity;
	
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	
	public class Camera3D implements IDevice
	{
		public var pointer:uint = 0;
		protected var lib:Library;
		protected var buffer:ByteArray;
		
		public var targetPtr:uint;
		public var hasTargetPtr:uint;
		public var fovPtr:uint;
		public var nearPtr:uint;
		public var farPtr:uint;
		public var fnfDirtyPtr:int;
		
		public var type:int;
		
		private var _target:Vector3D;
		private var _fov:Number;
		private var _near:Number;
		private var _far:Number;
		
		public var eye:Entity;
		
		public function get target():Vector3D
		{
			return _target;
		}
		
		public function set target(target:Vector3D):void
		{
			this._target = target;
			
			if (target)
			{
				buffer.position = targetPtr;
				buffer.writeFloat(target.x);
				buffer.writeFloat(target.y);
				buffer.writeFloat(target.z);
				
				buffer.position = hasTargetPtr;
				buffer.writeInt(1);
			}
			else
			{
				buffer.position = hasTargetPtr;
				buffer.writeInt(0);
			}
		}
		
		public function get fov():Number
		{
			return _fov;
		}
		
		public function set fov(value:Number):void
		{
			this._fov = value;
			
			buffer.position = fovPtr;
			buffer.writeFloat(value);
			
			buffer.position = fnfDirtyPtr;
			buffer.writeInt(1);
		}
		
		public function get far():Number
		{
			return _far;
		}
		
		public function set far(value:Number):void
		{
			this._far = value;
			
			buffer.position = farPtr;
			buffer.writeFloat(value);
			
			buffer.position = fnfDirtyPtr;
			buffer.writeInt(1);
		}
		
		public function get near():Number
		{
			return _near;
		}
		
		public function set near(value:Number):void
		{
			this._near = value;
			
			buffer.position = nearPtr;
			buffer.writeFloat(value);
			
			buffer.position = fnfDirtyPtr;
			buffer.writeInt(1);
		}
		
		public function Camera3D(type:int = 0, fov:Number = 90, near:Number = 100, far:Number = 5000, eye:Entity = null)
		{
			this.type = type;
			this._fov = fov;
			this._near = near;
			this._far = far;
			
			lib = Library.getInstance();
			buffer = lib.buffer;
			
			this.eye = eye == null ? new Entity(null, null, "camera") : eye;
		}
		
		public function initialize(devicePointer:uint):void
		{
			eye.initialize(0, 0);
			
			allotPtr(lib.alchemy3DLib.initializeCamera(devicePointer, eye.pointer, _fov, _near, _far));
		}
		
		public function allotPtr(ps:Array):void
		{
			pointer = ps[0];
			targetPtr = ps[1];
			fovPtr = ps[2];
			nearPtr = ps[3];
			farPtr = ps[4];
			fnfDirtyPtr = ps[5];
			hasTargetPtr = ps[6];
		}
		
		public function hover(mouseX:Number, mouseY:Number, camSpeed:Number):void
		{
			this.eye.x -= (this.eye.x - 1000 * mouseX) / camSpeed;
			this.eye.y -= (this.eye.y - 1000 * mouseY) / camSpeed;
		}
	}
}