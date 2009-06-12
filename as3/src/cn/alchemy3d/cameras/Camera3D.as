package cn.alchemy3d.cameras
{
	import cn.alchemy3d.lib.Alchemy3DLib;
	
	public class Camera3D
	{
		public var pointer:uint;
		
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
			
			var lib:Alchemy3DLib = Alchemy3DLib.getInstance();
			
			//初始化摄像机
			//返回该对象起始指针
			pointer = lib.alchemy3DLib.initializeCamera(type, zoom, focus, nearClip, farClip);
		}
	}
}