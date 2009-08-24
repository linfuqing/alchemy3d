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
	import flash.geom.Point;
	import flash.utils.ByteArray;

	public class Viewport3D extends Sprite implements IDevice
	{
		public var pointer:uint;
		public var mixedChannelPointer:uint;
		
		public var viewWidth:Number;
		public var viewHeight:Number;
		
		public var camera:Camera3D;
		public var scene:Scene3D;
		
		protected var mixedChannel:BitmapData;
		protected var wh:int;
		
		protected var lib:Library;
		protected var buffer:ByteArray;
		
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
			
			mixedChannel = new BitmapData(width, height, true, 0);
			addChild(new Bitmap(mixedChannel, PixelSnapping.NEVER, false));
			
			lib = Library.getInstance();
			buffer = lib.buffer;
			
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
			mixedChannelPointer = ps[1];
		}
		
		public function render():void
		{
			buffer.position = mixedChannelPointer;
			
			mixedChannel.lock();
//			mixedChannel.fillRect(mixedChannel.rect, 0);
			mixedChannel.setPixels(mixedChannel.rect, buffer);
			
//			buffer.position = gfx2Pointer;
//			
//			gfx2.lock();
//			gfx2.fillRect(gfx2.rect, 0);
//			gfx2.setPixels(gfx2.rect, buffer);
//			gfx2.unlock();
//			
//			gfx.merge(gfx2, gfx.rect, new Point(), 1, 1, 1, 1);

			mixedChannel.unlock();
		}
	}
}