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
		private var _loader:URLLoader;
		private var _material:Material;
		private var _texture:Texture;
		private var _renderMode:uint;
		private var _autoPlay:Boolean;
		private var _loop:Boolean;
		
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
		
		public function MD2(material:Material = null, texture:Texture = null, renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32, autoPlay:Boolean = false, loop:Boolean = false )
		{
			_material = material;
			_texture  = texture;
			_autoPlay = autoPlay;
			_loop = loop;
			
			this._renderMode = renderMode;
			
			super( "MD2_scene",new Mesh3D() );
		}
		
		public function load(url:String, fps:uint = 10):void
		{
			this.fps = fps;
			
			_loader = new URLLoader();
			_loader.dataFormat = URLLoaderDataFormat.BINARY;
			_loader.addEventListener(Event.COMPLETE, onLoadComplete);
			_loader.load(new URLRequest(url));
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
			
			writeAnimation( animation );
		}
		
		public function stop( animation:String = null ):void
		{
			if( !this.isPlayPointer )
			{
				return;
			}
			
			Library.memory.position = this.isPlayPointer;
			
			Library.memory.writeInt( FALSE );
			
			writeAnimation( animation );
		}
		
		private function writeAnimation( animation:String ):void
		{
			if( animation )
			{
				Library.memory.position = this.currentFrameNamePointer;
				Library.memory.writeUTFBytes( animation );
				
				Library.memory.position = this.dirtyPointer;
				Library.memory.writeInt( TRUE );
				
				Library.memory.position = this.nameLengthPointer;
				Library.memory.writeInt( animation.length );
			}
		}
		
		private function onLoadComplete(e:Event):void
		{
			var length:uint = _loader.data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(_loader.data, 0, length);
			
			_loader.data.clear();
			_loader.data = null;
			
			var ps:Array = Library.alchemy3DLib.initializeMD2( _pointer, fileBuffer, _material ? _material.pointer : NULL, _texture ? _texture.pointer : NULL, _renderMode, fps );
			
			this.isPlayPointer           = ps[0];
			this.loopPointer             = ps[1];
			this.dirtyPointer            = ps[2];
			this.currentFrameNamePointer = ps[3];
			this.nameLengthPointer       = ps[4];
			
			stop();
			
			if (_autoPlay)
				play(_loop);
				
			dispatchEvent(new Event(Event.COMPLETE));
		}
		
		private var isPlayPointer:uint = NULL;
		private var loopPointer:uint = NULL;
		private var dirtyPointer:uint = NULL;
		private var currentFrameNamePointer:uint = NULL;
		private var nameLengthPointer:uint       = NULL;
	}
}