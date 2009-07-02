package cn.alchemy3d.view
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.PixelSnapping;
	import flash.display.Sprite;

	public class View3D extends Sprite
	{
		public var pointer:uint;
		public var gfxPointer:uint;
		
		public var viewWidth:Number;
		public var viewHeight:Number;
		
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		protected var gfx:BitmapData;
		protected var wh:int;
		
		protected var lib:Alchemy3DLib;
		
		public function View3D(width:Number, height:Number, scene:Scene3D, camera:Camera3D)
		{
			super();
			
			this.scene = scene;
			this.camera = camera;
			
			viewWidth = width;
			viewHeight = height;
			wh = int(width) * int(height);
			
			gfx = new BitmapData(width, height, true, 0);
			addChild(new Bitmap(gfx, PixelSnapping.NEVER, false));
			
			lib = Alchemy3DLib.getInstance();
			
			//初始化场景
			//返回该对象起始指针
			var pointerArr:Array = lib.alchemy3DLib.initializeViewport(viewWidth, viewHeight, scene.pointer, camera.pointer);
			pointer = pointerArr[0];
			gfxPointer = pointerArr[1];
		}
		
		public function render():void
		{
			lib.buffer.position = gfxPointer;
			
//			for (var i:int = 10000; i < 20000; i ++)
//			{
//				trace(lib.buffer.readUnsignedInt());
//			}
			gfx.lock();
			gfx.fillRect(gfx.rect, 0);
			gfx.setPixels(gfx.rect, lib.buffer);
			gfx.unlock();
			//gfx.setPixel32(5, 5, 0xffffffff);
		}
	}
}