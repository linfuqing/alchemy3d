package cn.alchemy3d.cameras
{
	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	
	import flash.utils.ByteArray;
	
	public class Camera3D implements IDevice
	{
		public var pointer:uint = 0;
		protected var lib:Alchemy3DLib;
		protected var buffer:ByteArray;
		
		public var fovPointer:uint;
		public var nearPointer:uint;
		public var farPointer:uint;
		public var fnfDirtyPointer:int;
		
		public var type:int;
		
		private var _fov:Number;
		private var _near:Number;
		private var _far:Number;
		
		public var eye:DisplayObject3D;
		
		public function get fov():Number
		{
			return _fov;
		}
		
		public function set fov(value:Number):void
		{
			this._fov = value;
			
			buffer.position = fovPointer;
			buffer.writeFloat(value);
			
			buffer.position = fnfDirtyPointer;
			buffer.writeInt(1);
		}
		
		public function get far():Number
		{
			return _far;
		}
		
		public function set far(value:Number):void
		{
			this._far = value;
			
			buffer.position = farPointer;
			buffer.writeFloat(value);
			
			buffer.position = fnfDirtyPointer;
			buffer.writeInt(1);
		}
		
		public function get near():Number
		{
			return _near;
		}
		
		public function set near(value:Number):void
		{
			this._near = value;
			
			buffer.position = nearPointer;
			buffer.writeFloat(value);
			
			buffer.position = fnfDirtyPointer;
			buffer.writeInt(1);
		}
		
		public function Camera3D(type:int = 0, fov:Number = 90, near:Number = 100, far:Number = 5000, eye:DisplayObject3D = null)
		{
			this.type = type;
			this._fov = fov;
			this._near = near;
			this._far = far;
			
			lib = Alchemy3DLib.getInstance();
			buffer = lib.buffer;
			
			this.eye = eye == null ? new DisplayObject3D("camera") : eye;
			var arr:Array = lib.alchemy3DLib.createEntity(0, 0, 0, 0, 0, 0);
			this.eye.pointer = arr[0];
			this.eye.positionPtr = arr[1];
			this.eye.directionPtr = arr[2];
			this.eye.scalePtr = arr[3];
		}
		
		public function initialize(devicePointer:uint):void
		{
			var ps:Array = lib.alchemy3DLib.initializeCamera(devicePointer, eye.pointer, _fov, _near, _far);
			this.pointer = ps[0];
			this.fovPointer = ps[1];
			this.nearPointer = ps[2];
			this.farPointer = ps[3];
			this.fnfDirtyPointer = ps[4];
		}
		
		public function hover(mouseX:Number, mouseY:Number, camSpeed:Number):void
		{
			this.eye.x -= (this.eye.x - 1000 * mouseX) / camSpeed;
			this.eye.y -= (this.eye.y - 1000 * mouseY) / camSpeed;
		}
	}
}