package cn.alchemy3d.cameras
{
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	
	import flash.utils.ByteArray;
	
	public class Camera3D
	{
		public var pointer:uint;
		public var fovPointer:uint;
		public var nearPointer:uint;
		public var farPointer:uint;
		
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
			
			var ps:Array = lib.alchemy3DLib.initializeCamera(fov, near, far);
			this.pointer = ps[0];
			this.eye.pointer = ps[1];
			this.eye.positionPtr = ps[2];
			this.eye.directionPtr = ps[3];
			this.eye.scalePtr = ps[4];
			this.fovPointer = ps[5];
			this.nearPointer = ps[6];
			this.farPointer = ps[7];
		}
	}
}