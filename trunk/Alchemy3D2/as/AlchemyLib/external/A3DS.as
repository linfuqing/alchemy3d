package AlchemyLib.external
{
	import AlchemyLib.base.Library;
	import AlchemyLib.container.Entity;
	import AlchemyLib.render.RenderMode;
	import AlchemyLib.render.Texture;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;

	public class A3DS extends Entity
	{
		protected var a3dsPtr:uint;
		
		private var loader:URLLoader;
		private var tLoader:Loader;
		private var filename:String;
		private var path:String;
		
		protected var textures:Array = [];
		
		private var _renderMode:uint;
		
		public function A3DS(renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32)
		{
			_renderMode = renderMode;
			
			super("3DS_root");
		}
		
		public function load(url:String):void
		{
			buildFileInfo(url);
			
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.load(new URLRequest(url));
		}
		
		private function buildFileInfo(url:String):void
		{
			this.filename = url.split("\\").join("/");
			this.path = "";
				
			if( this.filename.indexOf("/") != -1 )
			{
				var parts:Array = this.filename.split("/");
				parts.pop();
				this.path = parts.join("/") + "/";
			}
		}
		
		protected function onLoadComplete(e:Event):void
		{
			loader.removeEventListener(Event.COMPLETE, onLoadComplete);
			
			var length:uint = loader.data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(loader.data, 0, length);
			
			var ps:Array = Library.alchemy3DLib.initialize3DS(loader.data, this.pointer, _renderMode);
			
			loader.data.clear();
			loader.data = null;
			
			a3dsPtr = ps[0];
			var mNum:int = ps[1];		//材质数目
			var tNum:int = ps[2];		//纹理数目
			var mPtr:uint = ps[3];		//材质列表指针
			var neOffset:int = ps[4];	//next指针偏移量
			var tOffset:int = ps[5];	//纹理偏移量
			var nOffset:int = ps[6];	//纹理url偏移量
			
			var tPtr:uint, nPtr:uint, value:uint = NaN;
			var url:ByteArray = new ByteArray();
			
			for (var i:int = 0; i < tNum; i ++)
			{
				tPtr = mPtr + tOffset;
				Library.memory.position = tPtr;
				tPtr = Library.memory.readUnsignedInt();	//取得纹理指针
				
				if (tPtr != 0)
				{
					nPtr = tPtr + nOffset;
					Library.memory.position = nPtr;
					nPtr = Library.memory.readUnsignedInt();	//取得纹理url指针
					
					Library.memory.position = nPtr;
					
					while ((value = Library.memory.readByte()) != 0) url.writeByte(value);
					
					textures.push({url:url.toString(), ptr:tPtr});
				}
				
				mPtr = mPtr + neOffset;
				Library.memory.position = mPtr;
				mPtr = Library.memory.readUnsignedInt();
			}
			
			if (textures.length) loadNextTexture();
		}
		
		private var queue:int = 0;
		
		protected function loadNextTexture():void
		{
			tLoader = new Loader();
			tLoader.contentLoaderInfo.addEventListener(Event.COMPLETE, onTextureLoadComplete);
			tLoader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			tLoader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			tLoader.load(new URLRequest(path + textures[queue].url));
		}
		
		protected function onProgress(e:ProgressEvent):void
		{
			dispatchEvent(e);
		}
		
		protected function ioErrorHandler(e:IOErrorEvent):void
		{
			throw new Error(e.toString());
		}
		
		protected function onTextureLoadComplete(e:Event):void
		{
			tLoader.contentLoaderInfo.removeEventListener(Event.COMPLETE, onTextureLoadComplete);
			tLoader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			tLoader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			
			var bitmapdata:BitmapData = Bitmap(tLoader.content).bitmapData;
			
			fillData(textures[queue].ptr, textures[queue].url, bitmapdata);
			
			queue ++;
			
			if (queue < textures.length)
			{
				loadNextTexture();
			}
		}
		
		protected function fillData(pointer:uint, name:String, bitmapdata:BitmapData):void
		{
			/*var byte:ByteArray = bitmapdata.getPixels(bitmapdata.rect);//new ByteArray();
			
			byte = bitmapdata.getPixels(bitmapdata.rect);
			byte.position = 0;
			
			var i:int = 0, j:int = bitmapdata.width * bitmapdata.height;
			var bitmapDataPtr:uint = Library.alchemy3DLib.applyForTmpBuffer(j * 4);
			Library.memory.position = bitmapDataPtr;
			
			for (; i < j; i ++)
				Library.memory.writeUnsignedInt(byte.readUnsignedInt());*/
			
			//Library.alchemy3DLib.initializeMipmap(pointer, bitmapdata.width, bitmapdata.height, bitmapDataPtr);
			Library.alchemy3DLib.setMipmap(pointer, Texture.initializeBitmap(bitmapdata), -1);

			bitmapdata.dispose();
			bitmapdata = null;
			
			Library.alchemy3DLib.loadComplete3DS(a3dsPtr);
		}
	}
}