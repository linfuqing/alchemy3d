package cn.alchemy3d.polygon
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.materials.AbstractMaterial;
	
	import flash.utils.ByteArray;
	
	public class Mesh extends Polygon
	{
		public function Mesh(name:String = "")
		{
			super(name);
			
			geomDirty = true;
			
			vertices = new Vector.<Vertex3D>();
			faces = new Vector.<Triangle3D>();
		}
		
		public var faces:Vector.<Triangle3D>;
		public var material:AbstractMaterial;
		public var vertices:Vector.<Vertex3D>;
		public var materialID:int;
		
		protected var geomDirty:Boolean;
		
		override public function clone():Polygon
		{
			var object:Mesh = new Mesh(name + "_cloned");
			
			object.faces = faces;
			object.vertices = vertices;
			
			object._direction = _direction.clone();
			object._position = _position.clone();
			object._scale = _scale.clone();
			
			var child:Polygon;
			for each(child in this.children)
				object.addChild(child.clone());
				
			return object;
		}
		
		override public function destory():void
		{
			var v:Vertex3D;
			var f:Triangle3D;
			
			for each (v in vertices)
				v = null;
			
			for each (f in faces)
			{
				f.destroy();
				f = null;
			}
			
			vertices.length = 0;
			vertices = null;
			faces.length = 0;
			faces = null;
		}
		
		/**
		 * 序列化
		 */
		override public function serialize(buffer:ByteArray, offset:int = 0):void
		{
			var v:Vertex3D;
			var f:Triangle3D;
			
			super.serialize(buffer, offset);
			
			if (geomDirty)
			{
				buffer.position = 80 + offset;
				
				buffer.writeDouble(vertices.length);	//1
				buffer.writeDouble(faces.length);		//2
				buffer.writeDouble(0);		//3
				buffer.writeDouble(0);			//4
				buffer.writeDouble(0);			//5
				buffer.writeDouble(0);			//6
				buffer.writeDouble(0);			//7
				buffer.writeDouble(0);			//8
				buffer.writeDouble(0);			//9
				
				buffer.position = 360 + offset;
			
				//data
				for each (v in vertices)
				{
					buffer.writeDouble(v.x);
					buffer.writeDouble(v.y);
					buffer.writeDouble(v.z);
				}
				
				for each (f in faces)
				{
					//顶点索引
					buffer.writeDouble(f.p0Index);
					buffer.writeDouble(f.p1Index);
					buffer.writeDouble(f.p2Index);
					//uv坐标
					buffer.writeDouble(f.uvtData[0]);
					buffer.writeDouble(f.uvtData[1]);
					buffer.writeDouble(f.uvtData[2]);
					buffer.writeDouble(f.uvtData[3]);
					buffer.writeDouble(f.uvtData[4]);
					buffer.writeDouble(f.uvtData[5]);
				}
				
				geomDirty = false;
			}
		}

	}
}