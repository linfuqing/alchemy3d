package cn.alchemy3d.render
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	
	public class Texture extends Pointer
	{
		//public var pointer:uint = 0;
		
		public var name:String;
		
		private var doubleSidePtr:uint;
		
		private var _ready:Boolean;
		private var _doubleSide:Boolean;
		private var loader:Loader;
		private var bitmapdata:BitmapData;
		
		public function get ready():Boolean
		{
			return this._ready;
		}
		
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
			this._ready = false;
			this.name = name;
			this.bitmapdata = bitmapdata;
					
			if (bitmapdata)
			{
				checkBitmapSize(bitmapdata.width, bitmapdata.height);
			}
			
			super();
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
		
		override protected function initialize():void
		{
			_pointer = Library.alchemy3DLib.initializeTexture(name);
			
			if (this.bitmapdata) fillData();
		}
		
		protected function fillData():void
		{
			var byte:ByteArray = new ByteArray();
			byte = bitmapdata.getPixels(bitmapdata.rect);
			byte.position = 0;
			
			var i:int = 0, j:int = bitmapdata.width * bitmapdata.height;
			var bitmapDataPtr:uint = Library.alchemy3DLib.applyForTmpBuffer(j * 4);
			Library.memory.position = bitmapDataPtr;
			
			for (; i < j; i ++)
				Library.memory.writeUnsignedInt(byte.readUnsignedInt());
			
			Library.alchemy3DLib.fillTextureData(_pointer, bitmapdata.width, bitmapdata.height, bitmapDataPtr);
			
			bitmapdata.dispose();
			bitmapdata = null;
			
			this._ready = true;
		}
		
		public function load(url:String):void
		{
			loader = new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			loader.load(new URLRequest(url));
		}
		
		protected function onProgress(e:ProgressEvent):void
		{
			dispatchEvent(e);
		}
		
		protected function ioErrorHandler(e:IOErrorEvent):void
		{
			throw new Error(e.toString());
		}
		
		protected function onLoadComplete(e:Event):void
		{
			loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, onLoadComplete);
			loader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			
			this.bitmapdata = Bitmap(loader.content).bitmapData;
			
			checkBitmapSize(bitmapdata.width, bitmapdata.height);
			
			fillData();
			
			dispatchEvent(e);
		}
	}
}