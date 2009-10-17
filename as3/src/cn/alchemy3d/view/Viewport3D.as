package cn.alchemy3d.view
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	import cn.alchemy3d.container.Scene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;

	public class Viewport3D extends Pointer
	{
		private var videoBufferPointer:uint;
		private var zBufferPointer:uint;
		private var cameraPointer:uint;
		private var scenePointer:uint;
		
		private var viewWidth:Number;
		private var viewHeight:Number;
		
		private var _camera:Camera3D;
		private var _scene:Scene3D;
		private var _container:Sprite;
		
		protected var video:Bitmap;
		protected var videoBuffer:BitmapData;
		protected var wh:int;
		
		private var _bgColor:int;
		
		public function get scene():Scene3D
		{
			return _scene;
		}
		
		public function get camera():Camera3D
		{
			return _camera;
		}
		
		public function get mouseX():Number
		{
			return video.mouseX - viewWidth * .5;
		}
		
		public function get mouseY():Number
		{
			return video.mouseY - viewHeight * .5;
		}
		
		public function set backgroundColor(color:uint):void
		{
			_bgColor = color;
			
			clearVB.position = 0;
			
			for (var i:int = 0; i < viewWidth * viewHeight; i ++ )
				clearVB.writeUnsignedInt(_bgColor);
		}
		
		public function set backgroundImage(bitmapData:BitmapData):void
		{
			if( !this._container )
			{
				return;
			}
			
			var t1:Number = viewWidth / bitmapData.width;
			var t2:Number = viewHeight / bitmapData.height;
			
			with ( this._container.graphics )
			{
				beginBitmapFill( bitmapData, new Matrix(t1, 0, 0, t2, 0 ,0), true, true );
				drawRect( 0, 0, this.viewWidth, this.viewHeight );
				endFill();
			}
		}
		
		private var clearVB:ByteArray;
		private var clearZB:ByteArray;
		
		public function Viewport3D(container:Sprite, width:int, height:int, camera:Camera3D, scene:Scene3D)
		{
			_container = container;
			_bgColor = 0;
			
			_camera = camera;
			_scene = scene;
			
			viewWidth = width;
			viewHeight = height;
			wh = width * height;
			
			videoBuffer = new BitmapData(width, height, false);
			video = new Bitmap(videoBuffer, "never", false);
			container.addChild(video);
			
			clearVB = new ByteArray();	
			clearZB = new ByteArray();
			
			for (var i:int = 0; i < width * height; i ++ )
				clearVB.writeUnsignedInt(_bgColor);
		
			for (i = 0; i < width * height * 4; i ++ )
				clearZB.writeByte(0);
			
			super();
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeViewport( viewWidth, viewHeight, _camera.pointer, _scene.pointer );

			_pointer			= ps[0];
			videoBufferPointer	= ps[1];
			zBufferPointer		= ps[2];
			cameraPointer		= ps[3];
			scenePointer		= ps[4];
		}
		
		public function render():void
		{
			//清空位图缓存
			Library.memory.position = videoBufferPointer;
			Library.memory.writeBytes(clearVB, 0, clearVB.length);
			
			//清空ZBuffer
			Library.memory.position = zBufferPointer;
			Library.memory.writeBytes(clearZB, 0, clearZB.length);
			
			Library.alchemy3DLib.render(pointer, getTimer());
			
			Library.memory.position = videoBufferPointer;
			//videoBuffer.lock();
			videoBuffer.setPixels(videoBuffer.rect, Library.memory);
			//videoBuffer.unlock();
		}
	}
}