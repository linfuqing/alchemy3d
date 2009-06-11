package cn.alchemy3d.objects
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.materials.AbstractMaterial;
	
	import flash.utils.ByteArray;
	
	public class Mesh3D extends DisplayObject3D
	{
		public function Mesh3D(name:String)
		{
			super(name);
			
			geomDirty = true;
			
			vertices = new Vector.<Vertex3D>();
			faces = new Vector.<Triangle3D>();
		}
		
		public var faces:Vector.<Triangle3D>;
		public var vertices:Vector.<Vertex3D>;
		
		protected var geomDirty:Boolean;
		
		override public function clone():DisplayObject3D
		{
			return null;
		}
	}
}