package cn.alchemy3d.objects
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.texture.Texture;
	
	public class Mesh3D extends Entity implements ISceneNode
	{
		public function Mesh3D(material:Material = null, texture:Texture = null, name:String = "")
		{
			super(material, texture, name);
			
			vertices = new Vector.<Vertex3D>();
			faces = new Vector.<Triangle3D>();
		}
		
		public var faces:Vector.<Triangle3D>;
		public var vertices:Vector.<Vertex3D>;
		public var tmpBuffPointer:uint;
		public var verticesPointer:uint;
		public var facesPointer:uint;
		
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
		
		override public function initialize(scenePtr:uint, parentPtr:uint):void
		{
			fillVerticesToBuffer();
			fillFacesToBuffer();
			
			var tPtr:uint = texture == null ? 0 : texture.pointer;
			var mPtr:uint = material == null ? 0 : material.pointer;
			
			allotPtr(lib.alchemy3DLib.initializeEntity(scenePtr, parentPtr, mPtr, tPtr, tmpBuffPointer, vertices.length, faces.length));
		}
		
		override public function allotPtr(ps:Array):void
		{
			super.allotPtr(ps);
			
			verticesPointer = ps[7];
			facesPointer = ps[8];
		}
		
		override public function clone():Entity
		{
			return null;
		}
	}
}