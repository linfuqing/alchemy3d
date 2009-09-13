package cn.alchemy3d.external
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.geom.Mesh3D;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.Texture;

	public class Primitives extends Entity
	{
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function Primitives( material:Material = null, texture:Texture = null )
		{
			_material = material ? material : new Material();
			_texture  = texture  ? texture  : new Texture();
			
			super("Primitives", new Mesh3D() );
		}
		
		public function toPlane( 
		width:Number = 500, 
		height:Number = 500, 
		segments_width:uint = 1, 
		segments_height:uint = 1 ):void
		{
			Library.alchemy3DLib.initializePrimitives( 
			mesh.pointer, 
			_material.pointer, 
			_texture.pointer, 
			width, 
			height,
			segments_width,
			segments_height );
		}
		
		private var _material:Material;
		private var _texture:Texture;
	}
}