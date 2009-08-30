package cn.alchemy3d.texture
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	
	public class Texture extends EventDispatcher
	{
		public var pointer:uint = 0;
		
		public var name:String;
		public var bitmapDataPtr:uint;
		public var doubleSidePtr:uint;
		
		private var _doubleSide:Boolean;
		private var loader:Loader;
		private var bitmapdata:BitmapData;
		
		public function get doubleSide():Boolean
		{
			return this._doubleSide;
		}
		
		public function set doubleSide(bool:Boolean):void
		{
			_doubleSide = bool;
			Library.memory.position = doubleSidePtr;
			
			if (bool)
				Library.memory.writeFloat(1);
			else
				Library.memory.writeFloat(0);
		}
		
		public function Texture(bitmapdata:BitmapData = null, name:String = "")
		{
			super();
			
			this.name = name;
			this.bitmapdata = bitmapdata;
			
			if (bitmapdata)
			{
				checkBitmapSize(bitmapdata.width, bitmapdata.height);
			}
			
			initialize();
		}
		
		protected function checkBitmapSize(w:Number, h:Number):void
		{
			var j:int;
				
			for (var i:int = 4; i <= 11; i ++)
			{
				j = 2 << i;
					
				if (bitmapdata.width == j || bitmapdata.height == j)
					break;
			}
				
			if (i == 11) Alchemy3DLog.error("位图大小不正确，高宽必须为2的幂");
		}
		
		public function initialize():void
		{
			if (!this.bitmapdata)
				return;
			
			var byte:ByteArray = bitmapdata.getPixels(bitmapdata.rect);
			byte.position = 0;
			
			var i:int = 0, j:int = bitmapdata.width * bitmapdata.height;
			bitmapDataPtr = Library.alchemy3DLib.applyForTmpBuffer(4, j);
			Library.memory.position = bitmapDataPtr;
			
			var uint32:uint;
			for (; i < j; i ++)
			{
				uint32 = byte.readUnsignedInt();
				
				Library.memory.writeUnsignedInt(uint32);
			}
			
			var ps:Array = Library.alchemy3DLib.initializeTexture(bitmapdata.width, bitmapdata.height, bitmapDataPtr);
			pointer = ps[0];
			Library.memory.position = ps[1];
		}
		
		public function load(url:String):void
		{
			loader = new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			loader.load(new URLRequest(url));
		}
		
		public function onProgress(e:ProgressEvent):void
		{
			dispatchEvent(e);
		}
		
		public function ioErrorHandler(e:IOErrorEvent):void
		{
			throw new Error(e.toString());
		}
		
		public function onLoadComplete(e:Event):void
		{
			loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, onLoadComplete);
			loader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			
			this.bitmapdata = Bitmap(loader.content).bitmapData;
			
			checkBitmapSize(bitmapdata.width, bitmapdata.height);
			
			initialize();
			
			dispatchEvent(e);
		}
	}
}