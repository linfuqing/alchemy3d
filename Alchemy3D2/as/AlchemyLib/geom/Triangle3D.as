package AlchemyLib.geom
{

	import AlchemyLib.render.Material;
	import AlchemyLib.render.RenderMode;
	import AlchemyLib.render.Texture;
	
	import flash.geom.Point;
	import flash.utils.ByteArray;
	
	public class Triangle3D
	{
		public function get material():Material
		{
			return _material;
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function Triangle3D(
		v0:uint,
		v1:uint,
		v2:uint,
		uva:TextureCoordinate, 
		uvb:TextureCoordinate, 
		uvc:TextureCoordinate, 
		material:Material, 
		texture:Texture = null, 
		renderMode:int = 0x000000 )
		{
			this.v0 = v0;
			this.v1 = v1;
			this.v2 = v2;
			
			uv0 = uva;
			uv1 = uvb;
			uv2 = uvc;
			
			
			_material = material ? material : Material.defaultMaterial;
			_texture  = texture;
			
			render_mode = renderMode;
		}
		
		static public function serialize(input:Triangle3D, data:ByteArray):void
		{
			data.writeUnsignedInt(input.v0);
			data.writeUnsignedInt(input.v1);
			data.writeUnsignedInt(input.v2);
			
			TextureCoordinate.serialize(input.uv0, data);
			TextureCoordinate.serialize(input.uv1, data);
			TextureCoordinate.serialize(input.uv2, data);
			
			data.writeUnsignedInt(input.render_mode);
		}
		
		static public function unserialze(data:ByteArray, material:Material, texture:Texture):Triangle3D
		{
			return new Triangle3D(
				data.readUnsignedInt(),
				data.readUnsignedInt(),
				data.readUnsignedInt(),
				TextureCoordinate.unserialze(data),
				TextureCoordinate.unserialze(data),
				TextureCoordinate.unserialze(data),
				material,
				texture,
				data.readUnsignedInt() );
		}
		
		public function toString():String
		{
			return "[Triangle3D]";
		}

		internal var v0:uint;
		internal var v1:uint;
		internal var v2:uint;
		
		internal var uv0:TextureCoordinate;
		internal var uv1:TextureCoordinate;
		internal var uv2:TextureCoordinate;
		
		internal var _material:Material;
		internal var _texture:Texture;
		
		internal var render_mode:int;
	}
}