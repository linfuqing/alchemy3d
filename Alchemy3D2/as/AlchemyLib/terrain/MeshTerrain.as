package AlchemyLib.terrain
{
	import AlchemyLib.base.Library;
	import AlchemyLib.container.Entity;
	import AlchemyLib.geom.Mesh3D;
	import AlchemyLib.render.Material;
	import AlchemyLib.render.RenderMode;
	import AlchemyLib.render.Texture;
	
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
		
		public function get terrainPointer():uint
		{
			return _terrainPointer;
		}
		
		public function MeshTerrain( map:BitmapData = null, material:Material = null, texture:Texture = null, renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32 )
		{
			var bitmap:BitmapData = map || new BitmapData( 128, 128 );
			
			if ( ! map ) bitmap.perlinNoise(256, 256, 4, Math.random() * 1000, true, true, 7, true);
			
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
								addressMode:int = 2 ):void
		{
			_terrainPointer = Library.alchemy3DLib.initializeTerrain( 
													_pointer, 
													_bmPointer,
													_material.pointer, 
													_texture.pointer, 
													width, 
													height,
													maxHeight,
													_renderMode,
													addressMode );
		}
		
		private var _map:Texture;
		private var _material:Material;
		private var _texture:Texture;
		private var _terrainPointer:uint;
	}
}