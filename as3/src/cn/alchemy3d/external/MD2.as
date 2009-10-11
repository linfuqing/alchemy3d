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
		
		public function MD2(material:Material = null, texture:Texture = null, renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32 )
		{
			_material = material;
			_texture  = texture;
			
			this.renderMode = renderMode;
			
			super( "MD2_scene",new Mesh3D() );
		}
		
		public function load(url:String, fps:uint = 10):void
		{
			this.fps = fps;
			
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onLoadComplete);
			loader.load(new URLRequest(url));
		}
		
		public function play( loop:Boolean = true, animation:String = null ):void
		{
			if( !this.isPlayPointer )
			{
				return;
			}
			
			Library.memory.position = this.loopPointer;
			
			Library.memory.writeInt( loop ? TRUE : FALSE );
			
			Library.memory.position = this.isPlayPointer;
			
			Library.memory.writeInt( TRUE );
			
			if( animation )
			{
				Library.memory.position = this.currentFrameNamePointer;
				Library.memory.writeUTFBytes( animation );
				
				Library.memory.position = this.dirtyPointer;
				Library.memory.writeInt( TRUE );
			}
		}
		
		public function stop():void
		{
			if( !this.isPlayPointer )
			{
				return;
			}
			
			Library.memory.position = this.isPlayPointer;
			
			Library.memory.writeInt( FALSE );
		}
		
		private function onLoadComplete(e:Event):void
		{
			var length:uint = loader.data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
			var ps:Array = Library.alchemy3DLib.initializeMD2( _pointer, mesh.pointer, fileBuffer, _material ? _material.pointer : NULL, _texture ? _texture.pointer : NULL, renderMode, fps );
			
			this.isPlayPointer           = ps[0];
			this.loopPointer             = ps[1];
			this.dirtyPointer            = ps[2];
			this.currentFrameNamePointer = ps[3];
			
			//stop();
			play(true,"wstnd");
		}
		
		private var isPlayPointer:uint = NULL;
		private var loopPointer:uint = NULL;
		private var dirtyPointer:uint = NULL;
		private var currentFrameNamePointer:uint = NULL;
	}
}