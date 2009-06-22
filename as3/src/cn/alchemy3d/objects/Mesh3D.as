package cn.alchemy3d.objects
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
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
		public var verticesPointer:uint;
		public var facesPointer:uint;
		public var materialID:int;
		
		protected var geomDirty:Boolean;
		
		public function fillVerticesToBuffer():ByteArray
		{
			//buffer.position = verticesPointer;
			var buff:ByteArray = new ByteArray();
			buff.endian = Endian.LITTLE_ENDIAN;
			
			var v:Vertex3D;
			
			for each (v in vertices)
			{
				buff.writeDouble(v.x);
				buff.writeDouble(v.y);
				buff.writeDouble(v.z);
				buff.writeDouble(v.w);
			}
			
			return buff;
		}
		
		public function fillFacesToBuffer():ByteArray
		{
			//buffer.position = facesPointer;
			var buff:ByteArray = new ByteArray();
			buff.endian = Endian.LITTLE_ENDIAN;
			
			var f:Triangle3D;
			
			for each (f in faces)
			{
				buff.writeDouble(f.p0Index);
				buff.writeDouble(f.p1Index);
				buff.writeDouble(f.p2Index);
				buff.writeDouble(f.uv0.x);
				buff.writeDouble(f.uv0.y);
				buff.writeDouble(f.uv1.x);
				buff.writeDouble(f.uv1.y);
				buff.writeDouble(f.uv2.x);
				buff.writeDouble(f.uv2.y);
			}
			
			return buff;
		}
		
		override public function clone():DisplayObject3D
		{
			return null;
		}
	}
}