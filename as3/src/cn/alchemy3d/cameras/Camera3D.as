package cn.alchemy3d.cameras
{
	import cn.alchemy3d.objects.DisplayObject3D;
	
	import flash.utils.ByteArray;

	public class Camera3D extends DisplayObject3D
	{
		public var type:String;
		public var zoom:Number;
		public var focus:Number;
		public var nearClip:Number;
		public var farClip:Number;
		public var targetID:int;
		public var dirty:Boolean;
		
		public function get fz():Number
		{
			return zoom * focus;
		}
		
		public function Camera3D()
		{
			super();
			
			type = CameraType.FREE;
			zoom = 10;
			focus = 40;
			nearClip = 10;
			farClip = 10000;
			targetID = -1;
			
			dirty = false;
		}
		
		/**
		 * 序列化
		 */
		override public function serialize():ByteArray
		{
			var buffer:ByteArray = super.serialize();
			
			if (dirty)
			{
				buffer.position = 0;
				
				buffer.writeDouble(zoom);			//0
				buffer.writeDouble(focus);	//1
				buffer.writeDouble(nearClip);		//2
				buffer.writeDouble(farClip);			//3
				buffer.writeDouble(targetID);			//4
				buffer.writeDouble(0);			//5
				buffer.writeDouble(0);			//6
				buffer.writeDouble(0);			//7
				buffer.writeDouble(0);			//8
				buffer.writeDouble(0);			//9
				
				dirty = false;
			}
			
			return buffer;
		}
	}
}