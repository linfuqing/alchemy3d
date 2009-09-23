package cn.alchemy3d.terrain
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.render.Texture;
	
	import flash.display.BitmapData;

	public class MeshTerrain extends Entity
	{
		private var _renderMode:uint;
		
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function MeshTerrain( map:Texture = null, material:Material = null, texture:Texture = null, renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32 )
		{
			_map      = map      ? map      : new Texture();
			
			if( !_map.ready )
			{
				var bitmap:BitmapData = new BitmapData( 64, 64 );
				
				bitmap.perlinNoise(64, 64, 8, Math.random() * 1000, true, true, 7, true);
				//bitmap.fillRect(bitmap.rect,0);
				_map.bitmapData = bitmap;
			}
			
			_material = material ? material : new Material();
			_texture  = texture  ? texture  : new Texture();
			
			_renderMode = renderMode;
			
			super("Terrain", new Mesh3D() );
		}
				
		public function buildOn( 
								width:Number  = 100000, 
								height:Number = 100000, 
								maxHeight:int = 10000 ):void
		{
			Library.alchemy3DLib.initializeTerrain( 
													_pointer, 
													_map.pointer,
													_material.pointer, 
													_texture.pointer, 
													width, 
													height,
													maxHeight,
													_renderMode );
		}
		
		private var _map:Texture;
		private var _material:Material;
		private var _texture:Texture;
	}
}