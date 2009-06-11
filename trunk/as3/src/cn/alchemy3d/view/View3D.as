package cn.alchemy3d.view
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.PixelSnapping;
	import flash.display.Sprite;

	public class View3D extends Sprite
	{
		public var viewWidth:Number;
		public var viewHeight:Number;
		
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		protected var gfx:BitmapData;
		
		public function View3D(width:Number, height:Number, scene:Scene3D, camera:Camera3D)
		{
			super();
			
			viewWidth = width;
			viewHeight = height;
			
			gfx = new BitmapData(width, height, true);
			addChild(new Bitmap(gfx, PixelSnapping.NEVER, false));
		}
	}
}