package cn.alchemy3d.cameras
{
	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.Entity;
	
	import flash.utils.ByteArray;
	
	public class Camera3D implements IDevice
	{
		public var pointer:uint = 0;
		protected var lib:Alchemy3DLib;
		protected var buffer:ByteArray;
		
		public var fovPtr:uint;
		public var nearPtr:uint;
		public var farPtr:uint;
		public var fnfDirtyPtr:int;
		
		public var type:int;
		
		private var _fov:Number;
		private var _near:Number;
		private var _far:Number;
		
		public var eye:Entity;
		
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
			
			lib = Alchemy3DLib.getInstance();
			buffer = lib.buffer;
			
			this.eye = eye == null ? new Entity(null, "camera") : eye;
		}
		
		public function initialize(devicePointer:uint):void
		{
			eye.allotPtr(lib.alchemy3DLib.initializeEntity(0, 0, 0, 0, 0, 0));
			
			allotPtr(lib.alchemy3DLib.initializeCamera(devicePointer, eye.pointer, _fov, _near, _far));
		}
		
		public function allotPtr(ps:Array):void
		{
			pointer = ps[0];
			fovPtr = ps[1];
			nearPtr = ps[2];
			farPtr = ps[3];
			fnfDirtyPtr = ps[4];
		}
		
		public function hover(mouseX:Number, mouseY:Number, camSpeed:Number):void
		{
			this.eye.x -= (this.eye.x - 1000 * mouseX) / camSpeed;
			this.eye.y -= (this.eye.y - 1000 * mouseY) / camSpeed;
		}
	}
}