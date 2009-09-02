package cn.alchemy3d.objects
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.texture.Texture;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	public class MD2 extends Entity
	{
		private var loader:URLLoader;
		
		public function MD2(material:Material=null, texture:Texture=null, name:String="")
		{
			super(material, texture, name);
		}
		
		public function load( URL:String ):void
		{
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, init);
			loader.load(new URLRequest(URL));
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
		
		override protected function allotPtr(ps:Array):void
		{
			super.allotPtr(ps);
			
			if( buffer )
			{
				Library.alchemy3DLib.test( pointer, buffer );
			}
		}
		
	}
}