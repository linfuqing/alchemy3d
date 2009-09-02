package cn.alchemy3d.objects
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.texture.Texture;

	public class MD2 extends Entity
	{
		public function MD2(material:Material=null, texture:Texture=null, name:String="")
		{
			super(material, texture, name);
		}
		
		public function loader( URL:String )
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
				Library.alchemy3DLib.test( pointer, pos );
			}
		}
		
	}
}