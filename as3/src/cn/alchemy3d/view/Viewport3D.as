package cn.alchemy3d.view
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.scene.Scene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.PixelSnapping;
	import flash.display.Sprite;

	public class Viewport3D extends Sprite implements IDevice
	{
		public var pointer:uint;
		public var gfxPointer:uint;
		
		public var viewWidth:Number;
		public var viewHeight:Number;
		
		public var camera:Camera3D;
		public var scene:Scene3D;
		
		protected var gfx:BitmapData;
		protected var wh:int;
		
		protected var lib:Library;
		
		override public function get mouseX():Number
		{
			return super.mouseX - viewWidth * .5;
		}
		
		override public function get mouseY():Number
		{
			return super.mouseY - viewHeight * .5;
		}
		
		public function set backgroundColor(color:uint):void
		{
			with (this.graphics)
			{
				beginFill(color, 1);
				drawRect(0, 0, this.width, this.height);
				endFill();
			}
		}
		
		public function Viewport3D(width:Number, height:Number, scene:Scene3D, camera:Camera3D)
		{
			super();
			
			if (scene.pointer == 0)
				throw new Error("场景没有被添加到设备列表");
			
			if (camera.pointer == 0)
				throw new Error("摄像机没有被添加到设备列表");
			
			this.scene = scene;
			this.camera = camera;
			
			viewWidth = width;
			viewHeight = height;
			wh = int(width) * int(height);
			
			gfx = new BitmapData(width, height, true, 0);
			addChild(new Bitmap(gfx, PixelSnapping.NEVER, false));
			
			lib = Library.getInstance();
			
			//初始化场景
			//返回该对象起始指针
			//var pointerArr:Array = lib.alchemy3DLib.initializeViewport(viewWidth, viewHeight, scene.pointer, camera.pointer);
			//pointer = pointerArr[0];
			//gfxPointer = pointerArr[1];
		}
		
		public function initialize(devicePointer:uint):void
		{
			allotPtr(lib.alchemy3DLib.initializeViewport(devicePointer, viewWidth, viewHeight, scene.pointer, camera.pointer));
		}
		
		public function allotPtr(ps:Array):void
		{
			pointer = ps[0];
			gfxPointer = ps[1];
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