package cn.alchemy3d.objects.external
{
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.texture.Texture;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	public class MD2 extends Entity
	{
		private var loader:URLLoader;
		private var material:Material;
		private var texture:Texture;
		
		public function MD2(material:Material = null, texture:Texture = null)
		{
<<<<<<< .mine
			this.material = material;
			this.texture  = texture;
			
			super( "MD2",new Mesh3D() );
=======
			super(material, texture);
>>>>>>> .r243
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
		
		private function onLoadComplete(e:Event):void
		{
			var length:uint = loader.data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
<<<<<<< .mine
			Library.alchemy3DLib.initializeMD2(mesh.pointer, fileBuffer, material ? material.pointer : 0, texture ? texture.pointer : 0, RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32);
=======
			var md2:Array = Library.alchemy3DLib.initializeMD2(fileBuffer, material ? material.pointer : 0, texture ? texture.pointer : 0, RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32);
			
			this.allotPtr(md2);
//			this.renderModePointer = mesh[0];
//			this.dirtyPointer      = mesh[1];
//			this.materialPtr       = mesh[2];
//			this.texturePtr        = mesh[3];
>>>>>>> .r243
		}
	}
}