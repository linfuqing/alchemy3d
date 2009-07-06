package cn.alchemy3d.cameras
{
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	
	import flash.utils.ByteArray;
	
	public class Camera3D
	{
		public var pointer:uint = 0;
		public var fovPointer:uint;
		public var nearPointer:uint;
		public var farPointer:uint;
		public var fnfDirtyPointer:int;
		
		public var type:int;
		public var _fov:Number;
		public var _near:Number;
		public var _far:Number;
		
		public var eye:DisplayObject3D;
		
		protected var lib:Alchemy3DLib;
		protected var buffer:ByteArray;
		
		public function get fov():Number
		{
			return _fov;
		}
		
		public function set fov(value:Number):void
		{
			this._fov = value;
			
			buffer.position = fovPointer;
			buffer.writeDouble(value);
			
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
			buffer.writeDouble(value);
			
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
			buffer.writeDouble(value);
			
			buffer.position = fnfDirtyPointer;
			buffer.writeInt(1);
		}
		
		public function Camera3D(type:int = 0, fov:Number = 90, near:Number = 100, far:Number = 5000)
		{
			this.type = type;
			this._fov = fov;
			this._near = near;
			this._far = far;
			
			lib = Alchemy3DLib.getInstance();
			buffer = lib.buffer;
			
			this.eye = new DisplayObject3D("camera");
		}
		
		public function initializeCamera(devicePointer:uint):void
		{
			var ps:Array = lib.alchemy3DLib.initializeCamera(devicePointer, fov, near, far);
			this.pointer = ps[0];
			this.eye.pointer = ps[1];
			this.eye.positionPtr = ps[2];
			this.eye.directionPtr = ps[3];
			this.eye.scalePtr = ps[4];
			this.fovPointer = ps[5];
			this.nearPointer = ps[6];
			this.farPointer = ps[7];
			this.fnfDirtyPointer = ps[8];
		}
		
		public function hover(mouseX:Number, mouseY:Number, camSpeed:Number):void
		{
			this.eye.x -= (this.eye.x - 1000 * mouseX) / camSpeed;
			this.eye.y -= (this.eye.y - 1000 * mouseY) / camSpeed;
		}
	}
}