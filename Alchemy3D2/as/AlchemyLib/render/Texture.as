package AlchemyLib.render
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.tools.Debug;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.display.Shader;
	import flash.display.ShaderJob;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.geom.Vector3D;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import flashx.textLayout.formats.Float;
	
	public class Texture extends Pointer
	{
		public var name:String;
		
		public function get perspectiveDist():Number
		{
			Library.memory.position = perspectiveDistPointer;
			
			return Library.memory.readFloat();
		}
		
		public function set perspectiveDist(value:Number):void
		{
			Library.memory.position = perspectiveDistPointer;
			
			Library.memory.writeFloat(value);
		}
		
		public function get perspectiveLPDist():Number
		{
			Library.memory.position = perspectiveLPDistPointer;
			
			return Library.memory.readFloat();
		}
		
		public function set perspectiveLPDist(value:Number):void
		{
			Library.memory.position = perspectiveLPDistPointer;
			
			Library.memory.writeFloat(value);
		}
		
		public function get alphaTestRef():int
		{
			Library.memory.position = alphaTestRefPointer;
			
			return Library.memory.readByte();
		}
		
		public function set alphaTestRef(byte:int):void
		{
			Library.memory.position = alphaTestRefPointer;
			
			Library.memory.writeByte(byte);
		}
		
		public function set mipLevels(value:int):void
		{
			_mipLevels = value;
			
			if(_bitmapdata)
			{
				var bitmapData:BitmapData = _bitmapdata;
				this.bitmapData = _bitmapdata;
				bitmapData.dispose();
			}
		}
		
		public function get mipLevels():int
		{
			Library.memory.position = mipLevelsPointer;
			
			return _mipLevels = Library.memory.readInt();
		}
		
		public function set bitmapData( bitmapData:BitmapData ):void
		{
			if(!bitmapData)
				return;
			
			if(_bitmapdata)
				_bitmapdata.dispose();
			
			if(bitmapData.transparent)
				_bitmapdata = bitmapData.clone();
			else
			{
				_bitmapdata = new BitmapData(bitmapData.width, bitmapData.height, true);
				_bitmapdata.copyPixels(bitmapData, _bitmapdata.rect, new Point);
			}

			checkBitmapSize(_bitmapdata.width, _bitmapdata.height);
			
			Library.alchemy3DLib.setMipmap(_pointer, initializeBitmap(_bitmapdata), _mipLevels);
		}
		
		public function get bitmapData():BitmapData
		{
			return _bitmapdata ? _bitmapdata.clone() : null;
		}
		
		public function Texture(name:String = null, bitmapdata:BitmapData = null, mipLevels:int = -1)
		{
			this.name = name;
			
			_mipLevels = mipLevels;
			
			super();
			
			this.bitmapData = bitmapdata;
		}
		
		public override function destroy(all:Boolean):void
		{
			if(_bitmapdata)
				_bitmapdata.dispose();
			
			Library.alchemy3DLib.destroyTexture(_pointer);
		}
		
		static public function initializeBitmap(bitmapdata:BitmapData):uint
		{
			var byte:ByteArray = new ByteArray();
			byte = bitmapdata.getPixels(bitmapdata.rect);
			byte.position = 0;
			
			var bitmapDataPtr:uint = Library.alchemy3DLib.applyForTmpBuffer(bitmapdata.width * bitmapdata.height * Library.intTypeSize );
			Library.memory.position = bitmapDataPtr;

//			byte.endian = Endian.LITTLE_ENDIAN;
//			byte.position = 0;
//			bitmapdata.setPixels(bitmapdata.rect, byte);
//			byte = bitmapdata.getPixels(bitmapdata.rect);
//			byte.position = 0;
//			Library.memory.writeBytes( byte );
			//var data:Vector.<uint> = bitmapdata.getVector(bitmapdata.rect);
			//var length:uint = data.length;
			//for(var i:uint = 0; i < length; i ++)
			//	Library.memory.writeUnsignedInt(data[i]);
			
			byte.position = 0;
			Library.memory.writeBytes(byte);
			
			Library.alchemy3DLib.swapTextureEndian(0, 0, bitmapdata.width, bitmapdata.height, bitmapDataPtr, bitmapdata.width);
			
			var bmPointer:uint = Library.alchemy3DLib.initializeBitmap(bitmapdata.width, bitmapdata.height, bitmapDataPtr);
			
			//Library.alchemy3DLib.swapTextureEndian(0, 0, bitmapdata.width, bitmapdata.height, bitmapDataPtr, bitmapdata.width);
			
			return bmPointer;
		}
		
		public function load(url:String):void
		{
			loader = new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			loader.load(new URLRequest(url));
		}
		
		public function getLevelDesc(level:uint, rect:Rectangle = null):uint
		{
			var info:Array = Library.alchemy3DLib.getTextureLevelDescMethod(_pointer, level);
			
			if(rect)
			{
				rect.x = 0;
				rect.y = 0;
				rect.width  = info[0];
				rect.height = info[1];
			}
			
			return info[2];
		}
		
		public function setPixels(rect:Rectangle, data:ByteArray, level:uint = 0):void
		{
			if(level > mipLevels)
			{
				Debug.warning("mip level error.");
				
				return;
			}
			
			var info:Array = Library.alchemy3DLib.getTextureLevelDesc(_pointer, level);
			var width:int = info[0];
			var height:int = info[1];
			var pointer:uint = info[2];
			var dataPosition:uint = 0;
			var memeryPosition:uint;
			
			if(rect.x < 0 || rect.y < 0 || rect.right > width || rect.bottom > height)
			{
				Debug.warning("rectangle size error.");
				
				return;
			}

			memeryPosition = pointer + rect.x + rect.y * width;
			if(rect.x == 0 && rect.width == width)
			{
				Library.memory.position = memeryPosition;
				data.position = 0;
				Library.memory.writeBytes(data);
			}
			else
			{
				for(var i:uint = 0; i < rect.height; i ++)
				{
					Library.memory.position = memeryPosition;
					data.position = 0;
					Library.memory.writeBytes(data, dataPosition, rect.width);
					
					memeryPosition += width;
					dataPosition += rect.width;
				}
			}
			
			Library.alchemy3DLib.swapTextureEndian(rect.x, rect.y, rect.width, rect.height, pointer, width);
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeTexture(name);
			
			_pointer				 = ps[0];
			perspectiveDistPointer	 = ps[1];
			perspectiveLPDistPointer = ps[2];
			alphaTestRefPointer      = ps[3];
			mipLevelsPointer         = ps[4];
		}
		
		protected function checkBitmapSize(w:Number, h:Number):void
		{
			var j:int;
			
			for (var i:int = 4; i <= 11; i ++)
			{
				j = 2 << i;
				
				if ( w == j || h == j )
					break;
			}
			
			if (i == 11) 
				Debug.error("位图大小不正确，高宽必须为2的幂");
		}
		
		protected function onProgress(e:ProgressEvent):void
		{
			dispatchEvent(e);
		}
		
		protected function ioErrorHandler(e:IOErrorEvent):void
		{
			Debug.warning( e.toString() );
			
			onLoadComplete(null);
		}
		
		protected function onLoadComplete(e:Event):void
		{
			loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, onLoadComplete);
			loader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler);
			
			var bitmap:Bitmap = Bitmap(loader.content);
			
			if(bitmap)
			{
				bitmapData = bitmap.bitmapData;
			
				bitmap.bitmapData.dispose();
			}
			
			dispatchEvent( e ? e : new Event(Event.COMPLETE) );
		}

		private var perspectiveDistPointer:uint;
		private var perspectiveLPDistPointer:uint;
		private var alphaTestRefPointer:uint;
		private var mipLevelsPointer:uint;
		
		private var _mipLevels:int;
		private var loader:Loader;
		private var address_mode:int;
		private var _bitmapdata:BitmapData;
	}
}