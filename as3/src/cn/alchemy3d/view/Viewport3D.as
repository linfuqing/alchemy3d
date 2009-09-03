package cn.alchemy3d.view
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.PixelSnapping;
	import flash.display.Sprite;

	public class Viewport3D extends Sprite
	{
		public var pointer:uint;
		public var videoBufferPointer:uint;
		public var cameraPointer:uint;
		public var scenePointer:uint;
		public var nRenderListPointer:uint;
		public var nCullListPointer:uint;
		public var nClippListPointer:uint;
		
		public var viewWidth:Number;
		public var viewHeight:Number;
		
		public var camera:Camera3D;
		public var scene:Scene3D;
		
		protected var videoBuffer:BitmapData;
		protected var wh:int;
		
		
		public function get nRenderList():int
		{
			Library.memory.position = nRenderListPointer;
			return Library.memory.readInt();
		}
		
		public function get nCullList():int
		{
			Library.memory.position = nCullListPointer;
			return Library.memory.readInt();
		}
		
		public function get nClippList():int
		{
			Library.memory.position = nClippListPointer;
			return Library.memory.readInt();
		}
		
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
				Alchemy3DLog.error("场景没有被添加到设备列表");
			
			if (camera.pointer == 0)
				Alchemy3DLog.error("摄像机没有被添加到设备列表");
			
			this.scene = scene;
			this.camera = camera;
			
			viewWidth = width;
			viewHeight = height;
			wh = int(width) * int(height);
			
			videoBuffer = new BitmapData(width, height, true, 0);
			addChild(new Bitmap(videoBuffer, PixelSnapping.NEVER, false));
			
			initialize();
		}
		
		public function initialize():void
		{
			allotPtr(Library.alchemy3DLib.initializeViewport(viewWidth, viewHeight, scene.pointer, camera.pointer));
		}
		
		public function allotPtr(ps:Array):void
		{
			pointer				= ps[0];
			videoBufferPointer	= ps[1];
			cameraPointer		= ps[2];
			scenePointer		= ps[3];
			nRenderListPointer	= ps[4];
			nCullListPointer	= ps[5];
			nClippListPointer	= ps[6];
		}
		
		public function render():void
		{
			Library.memory.position = videoBufferPointer;
			
			videoBuffer.lock();
			videoBuffer.setPixels(videoBuffer.rect, Library.memory);
			videoBuffer.unlock();
		}
	}
}