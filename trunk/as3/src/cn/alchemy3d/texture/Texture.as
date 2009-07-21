package cn.alchemy3d.texture
{
	import cn.alchemy3d.lib.Alchemy3DLib;
	
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
		protected var buffer:ByteArray;
		protected var lib:Alchemy3DLib;
		
		private var loader:Loader;
		private var bitmapdata:BitmapData;
		
		public function Texture(bitmapdata:BitmapData = null)
		{
			super();
			
			this.bitmapdata = bitmapdata;
			
			lib = Alchemy3DLib.getInstance();
			buffer = lib.buffer;
			
			initialize();
		}
		
		public function initialize():void
		{
			if (!this.bitmapdata)
				return;
			
			var ps:Array = lib.alchemy3DLib.initializeTexture(bitmapdata.width, bitmapdata.height);
			pointer = ps[0];
			buffer.position = ps[1];
			var b:ByteArray = bitmapdata.getPixels(bitmapdata.rect);
			buffer.writeBytes(b, 0, b.length);
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
			
			initialize();
			
			dispatchEvent(e);
		}
	}
}