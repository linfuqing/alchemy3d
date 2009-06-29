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
		public var tmpBuffPointer:uint;
		public var verticesPointer:uint;
		public var facesPointer:uint;
		public var materialID:int;
		
		protected var geomDirty:Boolean;
		
		public function fillVerticesToBuffer():void
		{
			buffer.position = tmpBuffPointer;
			
			var v:Vertex3D;
			
			for each (v in vertices)
			{
				buffer.writeDouble(v.x);
				buffer.writeDouble(v.y);
				buffer.writeDouble(v.z);
				buffer.writeDouble(v.w);
			}
		}
		
		public function fillFacesToBuffer():void
		{
			buffer.position = tmpBuffPointer + vertices.length * 4 * 8;
			
			var f:Triangle3D;
			
			for each (f in faces)
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
		
		protected function applyForTmpBuffer():void
		{
			tmpBuffPointer = lib.alchemy3DLib.applyForTmpBuffer(vertices.length * 4 + faces.length * 9);
		}
		
		override public function clone():DisplayObject3D
		{
			return null;
		}
	}
}