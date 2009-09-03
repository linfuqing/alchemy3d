package cn.alchemy3d.objects.external
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.texture.Texture;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	public class MD2 extends Entity
	{
		private var loader:URLLoader;
		
		public function MD2()
		{
			super("MD2_root");
		}
		
		public function load(url:String):void
		{
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.load(new URLRequest(url));
		}
		
		private var buffer:uint = 0;
		
		private function init(e:Event = null):void
		{
			loader.removeEventListener(Event.COMPLETE, init);
						
			var length:int = loader.data.length;
			
			buffer = Library.alchemy3DLib.applyForTmpBuffer(length);

			Library.memory.position = buffer;
			
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
		}
		
		protected function onLoadComplete(e:Event):void
		{
			var length:uint = loader.data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
			Library.alchemy3DLib.initializeMD2(this.pointer, fileBuffer, length);
		}
	}
}