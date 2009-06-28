package cn.alchemy3d.cameras
{
	import cn.alchemy3d.objects.DisplayObject3D;
	
	public class Camera3D extends DisplayObject3D
	{
		public var fovPointer:uint;
		public var nearClipPointer:uint;
		public var farClipPointer:uint;
		
		public var type:int;
		public var fov:Number;
		public var nearClip:Number;
		public var farClip:Number;
		
		public function Camera3D(type:int = 0, fov:Number = 60, nearClip:Number = 2, farClip:Number = 400)
		{
			super("camera");
			
			this.type = type;
			this.fov = fov;
			this.nearClip = nearClip;
			this.farClip = farClip;
		}
	}
}