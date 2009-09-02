package cn.alchemy3d.objects.external
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.objects.Entity;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	public class A3DS extends Entity
	{
		private var loader:URLLoader;
		
		public function A3DS()
		{
			super(null, null, "3DS_root");
		}
		
		public function load(url:String):void
		{
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.load(new URLRequest(url));
		}
		
		protected function onLoadComplete(e:Event):void
		{
			var length:uint = loader.data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
			Library.alchemy3DLib.initialize3DS(this.pointer, fileBuffer, length);
		}
	}
}