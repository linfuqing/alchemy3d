package cn.alchemy3d.view
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	import cn.alchemy3d.container.Scene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.DisplayObjectContainer;
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;

	public class Viewport3D extends Pointer
	{
		private var videoBufferPointer:uint;
		private var zBufferPointer:uint;
		private var cameraPointer:uint;
		private var scenePointer:uint;
		private var nRenderListPointer:uint;
		private var nCullListPointer:uint;
		private var nClippListPointer:uint;
		
		private var viewWidth:Number;
		private var viewHeight:Number;
		
		private var _camera:Camera3D;
		private var _scene:Scene3D;
		
		protected var video:Bitmap;
		protected var videoBuffer:BitmapData;
		protected var wh:int;
		
		public function get scene():Scene3D
		{
			return _scene;
		}
		
		public function get camera():Camera3D
		{
			return _camera;
		}
		
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
			if( !video.parent )
			{
				return;
			}
			
			var parent:DisplayObjectContainer = video.parent;
			
			while( parent && parent.parent )
			{
				parent = parent.parent;
			}
			
			if( parent is Sprite )
			{
				with ( ( parent as Sprite ).graphics )
				{
					beginFill( color, 1 );
					drawRect( 0, 0, this.width, this.height );
					endFill();
				}
			}
		}
		
		private var clearBA:ByteArray;
		
		public function Viewport3D(width:Number, height:Number, camera:Camera3D)
		{
			//if (scene.pointer == 0)
				//Alchemy3DLog.error("场景没有被添加到设备列表");
			
			//if (camera.pointer == 0)
				//Alchemy3DLog.error("摄像机没有被添加到设备列表");
			
			//this.scene = scene;
			_camera = camera;
			
			_scene = new Scene3D();
			
			viewWidth = width;
			viewHeight = height;
			wh = int(width) * int(height);
			
			videoBuffer = new BitmapData(width, height, false);
			video = new Bitmap(videoBuffer, "never", false);
			//addChild(video);
			
			clearBA = new ByteArray();
			for (var i:int = 0; i < width * height * 4; i ++ )
				clearBA.writeByte(0);
			
			super();
		}
		
		public function displayTo( root:DisplayObjectContainer ):void
		{
			root.addChild( video );
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeViewport( viewWidth, viewHeight, _camera.pointer, _scene.pointer );

			_pointer			= ps[0];
			videoBufferPointer	= ps[1];
			zBufferPointer		= ps[2]
			cameraPointer		= ps[3];
			scenePointer		= ps[4];
			nRenderListPointer	= ps[5];
			nCullListPointer	= ps[6];
			nClippListPointer	= ps[7];
		}
		
		public function render():void
		{
			//清空像素缓存
			Library.memory.position = videoBufferPointer;
			Library.memory.writeBytes(clearBA, 0, clearBA.length);
			
			//清空ZBuffer
			Library.memory.position = zBufferPointer;
			Library.memory.writeBytes(clearBA, 0, clearBA.length);
			
			Library.alchemy3DLib.render(pointer, getTimer());
			
			Library.memory.position = videoBufferPointer;
			//videoBuffer.lock();
			videoBuffer.setPixels(videoBuffer.rect, Library.memory);
			//videoBuffer.unlock();
		}
	}
}