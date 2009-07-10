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
				buffer.writeFloat(v.x);
				buffer.writeFloat(v.y);
				buffer.writeFloat(v.z);
				buffer.writeFloat(v.w);
			}
		}
		
		public function fillFacesToBuffer():void
		{
			buffer.position = tmpBuffPointer + vertices.length * 4 * sizeOfType;
			
			var f:Triangle3D;
			
			for each (f in faces)
			{
				buffer.writeFloat(f.p0Index);
				buffer.writeFloat(f.p1Index);
				buffer.writeFloat(f.p2Index);
				buffer.writeFloat(f.uv0.x);
				buffer.writeFloat(f.uv0.y);
				buffer.writeFloat(f.uv1.x);
				buffer.writeFloat(f.uv1.y);
				buffer.writeFloat(f.uv2.x);
				buffer.writeFloat(f.uv2.y);
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