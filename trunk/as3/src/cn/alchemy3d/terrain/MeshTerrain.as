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
		private var _bmPointer:uint
		
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function MeshTerrain( material:Material = null, texture:Texture = null, renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32 )
		{
			var bitmap:BitmapData = new BitmapData( 64, 64 );
			bitmap.perlinNoise(64, 64, 4, Math.random() * 1000, true, true, 7, true);
			
			_bmPointer = Texture.initializeBitmap(bitmap);
			
			_material = material ? material : new Material();
			_texture  = texture  ? texture  : new Texture();
			
			_renderMode = renderMode;
			
			super("Terrain", new Mesh3D() );
		}
				
		public function buildOn( 
								width:Number  = 1000, 
								height:Number = 1000, 
								maxHeight:int = 500,
								addressMode:int = TerrainAddressMode.MIRROR ):void
		{
			Library.alchemy3DLib.initializeTerrain( 
													_pointer, 
													_bmPointer,
													_material.pointer, 
													_texture.pointer, 
													width, 
													height,
													maxHeight,
													addressMode,
													_renderMode );
		}
		
		private var _map:Texture;
		private var _material:Material;
		private var _texture:Texture;
	}
}