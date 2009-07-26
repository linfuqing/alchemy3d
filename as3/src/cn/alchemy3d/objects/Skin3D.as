package cn.alchemy3d.objects
{
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.texture.Texture;

	public class Skin3D extends Entity
	{
		public function Skin3D(material:Material=null, texture:Texture=null, name:String="")
		{
			super(material, texture, name);
		}
		
	}
}