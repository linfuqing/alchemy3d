package cn.alchemy3d.objects.external
{
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Mesh3D;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.texture.Texture;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	public class MD2 extends Mesh3D
	{
		private var loader:URLLoader;
		
		public function MD2(material:Material = null, texture:Texture = null)
		{
			super(material, texture);
		}
		
		override protected function initialize():void
		{
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
			
			var md2:Array = Library.alchemy3DLib.initializeMD2(fileBuffer, material ? material.pointer : 0, texture ? texture.pointer : 0, RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32);
			
			this.allotPtr(md2);
//			this.renderModePointer = mesh[0];
//			this.dirtyPointer      = mesh[1];
//			this.materialPtr       = mesh[2];
//			this.texturePtr        = mesh[3];
		}
	}
}