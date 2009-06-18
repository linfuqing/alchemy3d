package cn.alchemy3d.objects
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	
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
		
		public function fillVerticesToBuffer(offset:uint):void
		{
			buffer.position = offset;
			
			var v:Vertex3D;
			var count:int = vertices.length;
			
			for each (v in count)
			{
				buffer.writeDouble(v.x);
				buffer.writeDouble(v.y);
				buffer.writeDouble(v.z);
			}
		}
		
		public function fillFacesToBuffer(offset:uint):void
		{
			buffer.position = offset;
			
			var f:Triangle3D;
			var count:int = faces.length;
			
			for each (f in count)
			{
				buffer.writeDouble(f.p0Index);
				buffer.writeDouble(f.p1Index);
				buffer.writeDouble(f.p2Index);
				buffer.writeDouble(f.uv0.x);
				buffer.writeDouble(f.uv0.y);
				buffer.writeDouble(f.uv1.x);
				buffer.writeDouble(f.uv1.y);
				buffer.writeDouble(f.uv2.x);
				buffer.writeDouble(f.uv2.y);
			}
		}
		
		override public function clone():DisplayObject3D
		{
			return null;
		}
	}
}