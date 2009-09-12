package cn.alchemy3d.external
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.render.Texture;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	public class MD2 extends Entity
	{
		private var loader:URLLoader;
		private var _material:Material;
		private var _texture:Texture;
		private var renderMode:uint;
		
		private var fps:uint;
		
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function set lightEnable(bool:Boolean):void
		{
			this.mesh.lightEnable = bool;
		}
		
		public function MD2(material:Material = null, texture:Texture = null, renderMode:uint = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32)
		{
			_material = material;
			_texture  = texture;
			
			this.renderMode = renderMode;
			
			super( "MD2",new Mesh3D() );
		}
		
		public function load(url:String, fps:uint = 10):void
		{
			this.fps = fps;
			
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
			
			Library.alchemy3DLib.initializeMD2(mesh.pointer, fileBuffer, _material ? _material.pointer : NULL, _texture ? _texture.pointer : NULL, renderMode, fps );
		}
	}
}