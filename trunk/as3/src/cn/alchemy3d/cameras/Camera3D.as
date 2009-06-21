package cn.alchemy3d.cameras
{
	public class Camera3D
	{
		public var pointer:uint;
		public var focusPointer:uint;
		public var zoomPointer:uint;
		public var nearClipPointer:uint;
		public var farClipPointer:uint;
		
		public var type:int;
		public var zoom:Number;
		public var focus:Number;
		public var nearClip:Number;
		public var farClip:Number;
		
		public function Camera3D(type:int = 0, zoom:Number = 10, focus:Number = 40, nearClip:Number = 10, farClip:Number = 10000)
		{
			this.type = type;
			this.zoom = zoom;
			this.focus = focus;
			this.nearClip = nearClip;
			this.farClip = farClip;
		}
	}
}