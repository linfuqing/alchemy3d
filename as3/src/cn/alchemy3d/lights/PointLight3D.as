package cn.alchemy3d.lights
{
	import flash.geom.ColorTransform;
	import flash.geom.Vector3D;
	
	public class PointLight3D extends Light3D
	{
		public function PointLight3D()
		{
			super();
			
			this.type = LightType.POINT_LIGHT;
		}
	}
}