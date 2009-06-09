package cn.alchemy3d.materials
{
	import flash.display.BitmapData;
	import flash.events.EventDispatcher;

	public class AbstractMaterial extends EventDispatcher
	{
		public var type:int;
		public var size:uint;
		public var name:String;
		public var maxU:Number;
		public var maxV:Number;
		public var ready:Boolean;
		
		/**
		 * 是否允许背面剔除
		 */
		public var doubleSide:Boolean;
		
		/**
		 * 贴图数据
		 */
		public var bitmapData:BitmapData;
		
		/**
		 * 平滑处理
		 */
		public var smooth:Boolean;
		
		/**
		 * 颜色
		 */
		public var color:uint;
		
		/**
		 * 透明值
		 * (范围:0 - 255)
		 */
		public var alpha:int;
		
		/**
		 * 颜色
		 */
		public var thickness:Number;
		
		public var slash:Boolean;
		

		public function AbstractMaterial(name:String="")
		{
			super();

			this.type = -1;
			this.size = 1;
			this.alpha = 255;
			this.name = name;
			this.maxU = this.maxV = 1;
			this.doubleSide = false;
		}

		public function clone():AbstractMaterial
		{
			return new AbstractMaterial(name + "cloned");
		}
		
		public function destory():void
		{
		}
	}
}