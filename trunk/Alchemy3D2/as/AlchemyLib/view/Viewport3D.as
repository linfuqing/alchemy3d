package AlchemyLib.view
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.container.Scene3D;
	import AlchemyLib.tools.Debug;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.geom.Matrix;
	import flash.geom.Rectangle;
	import flash.system.ApplicationDomain;
	import flash.utils.ByteArray;

	public class Viewport3D extends Pointer
	{
		private var videoBufferPointer:uint;
		private var zBufferPointer:uint;
		private var cameraPointer:uint;
		private var scenePointer:uint;
		private var sortTransListPointer:uint;
		
		private var viewWidth:Number;
		private var viewHeight:Number;
		
		private var _camera:Camera3D;
		private var _scene:Scene3D;
		private var _container:Sprite;
		
		protected var _video:Bitmap;
		protected var _videoBuffer:BitmapData;
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
		
		public function get alphaMixed():Boolean
		{
			Library.memory.position = sortTransListPointer;
			return Library.memory.readInt() ? true : false;
		}
		
		public function set alphaMixed(value:Boolean):void
		{
			Library.memory.position = sortTransListPointer;
			Library.memory.writeInt(value ? TRUE : FALSE);
		}
		
		public function get mouseX():Number
		{
			return _video.mouseX - viewWidth * .5;
		}
		
		public function get mouseY():Number
		{
			return _video.mouseY - viewHeight * .5;
		}
		
		public function get width():Number
		{
			return viewWidth;
		}
		
		public function get height():Number
		{
			return viewHeight;
		}
		
		public function set backgroundColor(color:uint):void
		{
			_bgColor = (color << 24) | ( (color << 8) & 0xff0000 ) | ( (color >> 8) & 0xff00 ) | (color >> 24);
			
			clearVB.position = 0;
			
			for (var i:int = 0; i < viewWidth * viewHeight; i ++ )
				clearVB.writeUnsignedInt(_bgColor);
		}
		
		public function get video():Bitmap
		{
			return _video;
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
			
			_videoBuffer = new BitmapData(width, height, false);
			_video = new Bitmap(_videoBuffer, "never", false);
			container.addChild(_video);
			
			clearVB = new ByteArray();	
			clearZB = new ByteArray();
			
			backgroundDirtyRect = null;
			
			for (var i:int = 0; i < width * height; i ++ )
				clearVB.writeUnsignedInt(_bgColor);
		
			for (i = 0; i < width * height * 4; i ++ )
				clearZB.writeByte(0);
			
			super();
		}
		
		public function setBackgroundImage(rect:Rectangle, data:ByteArray):void
		{
			if(rect.x < 0 || rect.y < 0 || rect.right > viewWidth || rect.bottom > viewHeight)
			{
				Debug.warning("rectangle size error.");
				
				return;
			}
			
			var dataPosition:uint = 0;
			var memeryPosition:uint;
			
			memeryPosition = rect.x + rect.y * viewWidth;
			if(rect.x == 0 && rect.width == viewWidth)
			{
				clearVB.position = memeryPosition;
				clearVB.writeBytes(data);
			}
			else
			{
				for(var i:uint = 0; i < rect.height; i ++)
				{
					Library.memory.position = memeryPosition;
					Library.memory.writeBytes(data, dataPosition + data.position, rect.width);
					
					memeryPosition += viewWidth;
					dataPosition += rect.width;
				}
			}
			//clearVB.writeBytes(data);
			
			backgroundDirtyRect = rect;
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeViewport( viewWidth, viewHeight, _camera.pointer, _scene.pointer );

			_pointer			 = ps[0];
			videoBufferPointer	 = ps[1];
			zBufferPointer		 = ps[2];
			cameraPointer		 = ps[3];
			scenePointer		 = ps[4];
			sortTransListPointer = ps[5];
		}
		
		public function render(time:int):void
		{
			//清空位图缓存
			Library.memory.position = videoBufferPointer;
			Library.memory.writeBytes(clearVB, 0, clearVB.length);
			
			if(backgroundDirtyRect)
			{
				Library.alchemy3DLib.swapTextureEndian(backgroundDirtyRect.x, backgroundDirtyRect.y, backgroundDirtyRect.width, backgroundDirtyRect.height, videoBufferPointer, viewWidth);
				
				//backgroundDirtyRect = null;
			}
			
			//清空ZBuffer
			
			if( !(numFrames & 7) )
			{
				Library.memory.position = zBufferPointer;
				Library.memory.writeBytes(clearZB, 0, clearZB.length);
			}
			
			//trace(mouseX);
//			trace( Library.alchemy3DLib.render(pointer, getTimer(), video.mouseX, video.mouseY ) );
			Library.alchemy3DLib.render(pointer, time, _video.mouseX, _video.mouseY );

			Library.memory.position = videoBufferPointer;
			//videoBuffer.lock();
			_videoBuffer.setPixels(_videoBuffer.rect, Library.memory);
			//videoBuffer.unlock();
			
			numFrames ++;
		}
		
		private var numFrames:int = 0;
		private var backgroundDirtyRect:Rectangle;
	}
}