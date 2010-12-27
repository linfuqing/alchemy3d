package AlchemyLib.external
{
	import AlchemyLib.animation.AnimationObject;
	import AlchemyLib.animation.MorphAnimation;
	import AlchemyLib.base.Library;
	import AlchemyLib.container.Entity;
	import AlchemyLib.geom.Mesh3D;
	import AlchemyLib.render.Material;
	import AlchemyLib.render.RenderMode;
	import AlchemyLib.render.Texture;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;

	public class MD2 extends AnimationObject
	{
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
			
			this._renderMode = renderMode;
			
			super(autoPlay, loop);
		}
		
		public function load(url:String, fps:uint = 10):void
		{
			this.fps = fps;
			
			_loader = new URLLoader();
			_loader.dataFormat = URLLoaderDataFormat.BINARY;
			_loader.addEventListener(Event.COMPLETE, onLoadComplete);
			_loader.load(new URLRequest(url));
		}
		
		public function parse(data:*):void
		{
			parseData( ByteArray(data) );
		}
		
		private function onLoadComplete(e:Event):void
		{
			parseData(_loader.data);
		}
		
		private function parseData(data:ByteArray):void
		{
			var length:uint = data.length;
			
			var fileBuffer:uint = Library.alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = fileBuffer;
			Library.memory.writeBytes(data, 0, length);
			
			mesh = new Mesh3D();
			
			morphAnimation = new MorphAnimation(null, null, null, null);
			
			Library.alchemy3DLib.initializeMD2( _pointer, fileBuffer, _material ? _material.pointer : NULL, _texture ? _texture.pointer : NULL, _renderMode, fps );
			
			dispatchEvent(new Event(Event.COMPLETE));
		}
		
		private var _loader:URLLoader;
		private var _material:Material;
		private var _texture:Texture;
		private var _renderMode:uint;
		private var _autoPlay:Boolean;
		private var _loop:Boolean;
		
		private var fps:uint;
	}
}