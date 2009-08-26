package cn.alchemy3d.objects
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.texture.Texture;
	
	import flash.geom.Point;
	import flash.geom.Vector3D;
	import flash.utils.Dictionary;
	
	public class Mesh3D extends Entity implements ISceneNode
	{
		public function Mesh3D(material:Material, texture:Texture = null, name:String = "")
		{
			super(material == null ? new Material() : material, texture, name);
			
			vertices = new Vector.<Vertex3D>();
			faces = new Vector.<Triangle3D>();
		}
		
		public var faces:Vector.<Triangle3D>;
		public var vertices:Vector.<Vertex3D>;
		
		public var meshBuffPointer:uint;
		public var verticesPointer:uint;
		public var facesPointer:uint;
		public var dirtyPointer:uint;
		public var renderModePointer:uint;
		
		public var sizeOfVertex:int;
		public var sizeOfFace:int;
		
		private const vSize:uint = 5;
		private const fSize:uint = 9;
		
		public function get nFaces():int
		{
			return faces.length;
		}
		
		public function get nVertices():int
		{
			return vertices.length;
		}
		
		/**
		 * 设置渲染模式
		 */
		public function set renderMode(mode:uint):void
		{
			if (!checkInitialized()) return;
			
			buffer.position = renderModePointer;
			buffer.writeUnsignedInt(mode);
		}
		
		public function setVertices(index:int, v:Vector3D):void
		{
			if (!checkInitialized()) return;
			
			vertices[index].x = v.x;
			vertices[index].y = v.y;
			vertices[index].z = v.z;
			buffer.position = meshBuffPointer + index * sizeOfInt * vSize;
			buffer.writeFloat(v.x);
			buffer.writeFloat(v.y);
			buffer.writeFloat(v.z);
			
			buffer.position = dirtyPointer;
			buffer.writeUnsignedInt(1);
		}
		
		public function setVerticesX(index:int, value:Number):void
		{
			if (!checkInitialized()) return;
			
			vertices[index].x = value;
			buffer.position = meshBuffPointer + index * sizeOfInt * vSize;
			buffer.writeFloat(value);
			
			buffer.position = dirtyPointer;
			buffer.writeUnsignedInt(1);
		}
		
		public function setVerticesY(index:int, value:Number):void
		{
			if (!checkInitialized()) return;
			
			vertices[index].y = value;
			buffer.position = meshBuffPointer + index * sizeOfInt * vSize + sizeOfInt;
			buffer.writeFloat(value);
			
			buffer.position = dirtyPointer;
			buffer.writeUnsignedInt(1);
		}
		
		public function setVerticesZ(index:int, value:Number):void
		{
			if (!checkInitialized()) return;
			
			vertices[index].z = value;
			//获得顶点缓冲区的起始指针
			buffer.position = meshBuffPointer + index * sizeOfInt * vSize + sizeOfInt * 2;
			//写入数据
			buffer.writeFloat(value);
			
			buffer.position = dirtyPointer;
			buffer.writeUnsignedInt(1);
		}
		
		public function fillVerticesToBuffer():void
		{
			buffer.position = meshBuffPointer;
			
			var v:Vertex3D;
			
			for each (v in vertices)
			{
				v.pointer = buffer.position + 4 * sizeOfInt;	//4代表保存指针的偏移量
				
				buffer.writeFloat(v.x);
				buffer.writeFloat(v.y);
				buffer.writeFloat(v.z);
				buffer.writeFloat(v.w);
				buffer.writeUnsignedInt(0);	//for saving pointer
			}
		}
		
		public function fillFacesToBuffer():void
		{
			buffer.position = meshBuffPointer + vertices.length * vSize * sizeOfInt;
			
			var f:Triangle3D;
			
			for each (f in faces)
			{
				buffer.writeUnsignedInt(f.v0.pointer);
				buffer.writeUnsignedInt(f.v1.pointer);
				buffer.writeUnsignedInt(f.v2.pointer);
				buffer.writeFloat(f.uv0.x);
				buffer.writeFloat(f.uv0.y);
				buffer.writeFloat(f.uv1.x);
				buffer.writeFloat(f.uv1.y);
				buffer.writeFloat(f.uv2.x);
				buffer.writeFloat(f.uv2.y);
			}
		}
		
		protected function applyForMeshBuffer():void
		{
			meshBuffPointer = lib.alchemy3DLib.applyForTmpBuffer(sizeOfInt, (vertices.length * vSize + faces.length * fSize) * sizeOfInt);
		}
		
		override public function initialize(scene:Scene3D):void
		{
			fillVerticesToBuffer();
			fillFacesToBuffer()
			
			super.initialize(scene);
		}
		
		override protected function callAlchemy():Array
		{
			var tPtr:uint = texture == null ? 0 : texture.pointer;
			var mPtr:uint = material == null ? 0 : material.pointer;
			var parentPtr:uint = parent == null ? 0 : parent.pointer;
			var scenePtr:uint = scene == null ? 0 : scene.pointer;
			
			return lib.alchemy3DLib.initializeEntity(scenePtr, parentPtr, mPtr, tPtr, meshBuffPointer, vertices.length, faces.length);
		}
		
		override protected function allotPtr(ps:Array):void
		{
			super.allotPtr(ps);
			
			renderModePointer	= ps[5];
			dirtyPointer		= ps[6];
			materialPtr			= ps[7];
			texturePtr			= ps[8];
		}
		
		override public function clone():Entity
		{
			return null;
		}
		
		public function flipFaces():void
		{
			for each(var f:Triangle3D in this.faces)
			{
				var tmp:Vertex3D = f.v0;
				f.v0 = f.v2;
				f.v2 = tmp;
				
				var tmpUV:Point = f.uv0;
				f.uv0 = f.uv2;
				f.uv2 = tmpUV;
			}
		}
		
		/**
		* 合并共有顶点
		*/
		protected function mergeVertices():void
		{
			var uniqueDic:Dictionary = new Dictionary();
			var indexDic:Dictionary = new Dictionary();
			var uniqueList:Vector.<Vertex3D> = new Vector.<Vertex3D>();
	
			//找出共有顶点
			var v:Vertex3D;
			var vu:Vertex3D;
			for each(v in this.vertices)
			{
				for each(vu in uniqueDic)
				{
					if (v.equals(vu))
					{
						uniqueDic[v] = vu;
						break;
					}
				}
				
				if( ! uniqueDic[v] )
				{
					uniqueDic[v] = v;
					uniqueList.push(v);
				}
			}

			this.vertices = uniqueList;

			//更新面信息
			var f:Triangle3D;
			for each(f in this.faces)
			{
				f.v0 = uniqueDic[f.v0];
			    f.v1 = uniqueDic[f.v1];
			    f.v2 = uniqueDic[f.v2];
			}
		}
	}
}