package cn.alchemy3d.terrain
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.Texture;
	
	import flash.display.BitmapData;

	public class MeshTerrain extends Entity
	{
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function MeshTerrain( map:Texture = null, material:Material = null, texture:Texture = null )
		{
			_map      = map      ? map      : new Texture();
			
			if( !_map.ready )
			{
				var bitmap:BitmapData = new BitmapData( 128, 128 );
				
				bitmap.perlinNoise(128,128,4,Math.random()*1000,true,false,4,true);
				
				_map.bitmapData = bitmap;
			}
			
			_material = material ? material : new Material();
			_texture  = texture  ? texture  : new Texture();
			
			super("Primitives", new Mesh3D() );
		}
				
		public function buildOn( 
		width:Number  = 100000, 
		height:Number = 100000, 
		maxHeight:int = 10000 ):void
		{
			Library.alchemy3DLib.initializeTerrain( 
			mesh.pointer, 
			_map.pointer,
			_material.pointer, 
			_texture.pointer, 
			width, 
			height,
			maxHeight );
			
			//this.rotationX = 90;
		}
		
		private var _map:Texture;
		private var _material:Material;
		private var _texture:Texture;
	}
}