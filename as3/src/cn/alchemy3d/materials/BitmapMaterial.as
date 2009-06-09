package cn.alchemy3d.materials
{
	import cn.rc3dx.events.LoadEvent;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLRequest;

	/**
	 * 贴图材质
	 */
	public class BitmapMaterial extends AbstractMaterial
	{
		protected var url:String, filename:String;
		protected var loader:Loader;
		
		/**
		 * 构造函数
		 * 
		 * @param _bitmapData 图像数据
		 * @param smooth 平滑
		 */
		public function BitmapMaterial(bitmapData:BitmapData = null, smooth:Boolean = false, alpha:int = 255)
		{
			super();
			
			this.type = 2;
			this.alpha = alpha;
			this.smooth = smooth;
			if (bitmapData)
			{
				this.size = bitmapData.width * bitmapData.height;
				this.bitmapData = bitmapData;
				this.ready = true;
			}
			else
				this.ready = false;
		}
		
		/**
		 * [internal-use]返回当前材质的一个副本
		 */
		override public function clone():AbstractMaterial
		{
			return new BitmapMaterial(bitmapData, smooth);
		}
		
		/**
		 * 销毁材质
		 */
		override public function destory():void
		{
			this.bitmapData.dispose();
		}
		
		/**
		 * 从指定URL加载位图
		 * 
		 * @param url url
		 */
		public function load(url:String):void
		{
			this.url = url;
			this.filename = buildFileInfo(url);
			
			loader = new Loader();
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loadComplete);
			loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, loadError);
			loader.load(new URLRequest(url));
		}
		
		private function buildFileInfo(url:String):String
		{
			var parts:Array = url.split("/");
			return String( parts.pop() );
		}
		
		//--------------load bitmap--------------
		private function onProgress(e:ProgressEvent):void
		{
			dispatchEvent(new LoadEvent(LoadEvent.PROGRESS, filename, e.bytesLoaded, e.bytesTotal));
		}
		
		private function loadComplete(e:Event):void
		{
			loader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, loadError);
			loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, loadComplete);
			loader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			
			this.bitmapData = (loader.content as Bitmap).bitmapData;
			dispatchEvent(new LoadEvent(LoadEvent.TEXTURE_LOAD_COMPLETE, filename));
		}
		
		private function loadError(e:Event):void
		{
			loader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, loadError);
			loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, loadComplete);
			loader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			
			dispatchEvent(new LoadEvent(LoadEvent.LOAD_ERROR, filename));
		}
		//--------------end load bitmap--------------
	}
}