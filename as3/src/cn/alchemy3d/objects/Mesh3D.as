package cn.alchemy3d.objects
{

	import cn.alchemy3d.events.LoadEvent;
	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.texture.Texture;
	
	import flash.geom.Point;
	import flash.geom.Vector3D;
	import flash.utils.Dictionary;
	
	public class Mesh3D extends Entity
	{
		public function Mesh3D(material:Material = null, texture:Texture = null, name:String = "")
		{
			this._material = material == null ? new Material() : material;
			this._texture = texture;
			
			this._lightEnable = false;
			
			vertices = new Vector.<Vertex3D>();
			faces = new Vector.<Triangle3D>();
			
			super(name);
			
			if (texture)
			{
				if (texture.ready)
					Library.alchemy3DLib.onTextureReady(this.pointer, texture.pointer);
				else
					texture.addEventListener(LoadEvent.TEXTURE_READY, onTextureReady);
			}
		}
		
		public var faces:Vector.<Triangle3D>;
		public var vertices:Vector.<Vertex3D>;
		
		public var meshBuffPointer:uint;
		public var verticesPointer:uint;
		public var facesPointer:uint;
		public var vDirtyPointer:uint;
		public var fDirtyPointer:uint;
		
		private var _material:Material;
		private var _texture:Texture;
		private var _lightEnable:Boolean;
		
		private const vSize:uint = 5;
		private const fSize:uint = 12;
		
		public function get nFaces():int
		{
			return faces.length;
		}
		
		public function get nVertices():int
		{
			return vertices.length;
		}
		
		public function get material():Material
		{
			return _material;
		}
		
		public function set material(material:Material):void
		{
			for (var i:int = 0; i < faces.length; i ++)
			{
				Library.memory.position = meshBuffPointer + (vSize * vertices.length + 10 + i * fSize) * sizeOfInt;
				
				if (material)
					Library.memory.writeUnsignedInt(material.pointer);
				else
					Library.memory.writeUnsignedInt(0);
			}
				
			Library.memory.position = fDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		public function get texture():Texture
		{
			return _texture;
		}
		
		public function set texture(texture:Texture):void
		{
			for (var i:int = 0; i < faces.length; i ++)
			{
				Library.memory.position = meshBuffPointer + (vSize * vertices.length + 11 + i * fSize) * sizeOfInt;
				
				if (texture)
					Library.memory.writeUnsignedInt(texture.pointer);
				else
					Library.memory.writeUnsignedInt(0);
			}
				
			Library.memory.position = fDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		/**
		 * 设置渲染模式
		 */
		public function set renderMode(mode:uint):void
		{
			for (var i:int = 0; i < faces.length; i ++)
			{
				Library.memory.position = meshBuffPointer + (vSize * vertices.length + 9 + i * fSize) * sizeOfInt;
				Library.memory.writeUnsignedInt(mode);
			}
			
			Library.memory.position = fDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		public function get lightEnable():Boolean
		{
			return _lightEnable;
		}
		
		public function set lightEnable(bool:Boolean):void
		{
			_lightEnable = bool;
			
			Library.memory.position = lightEnablePtr;
			
			if (bool)
				Library.memory.writeUnsignedInt(1);
			else
				Library.memory.writeUnsignedInt(0);
		}
		
		public function setVertices(index:int, v:Vector3D):void
		{
			vertices[index].x = v.x;
			vertices[index].y = v.y;
			vertices[index].z = v.z;
			Library.memory.position = meshBuffPointer + index * sizeOfInt * vSize;
			Library.memory.writeFloat(v.x);
			Library.memory.writeFloat(v.y);
			Library.memory.writeFloat(v.z);
			
			Library.memory.position = vDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		public function setVerticesX(index:int, value:Number):void
		{
			vertices[index].x = value;
			Library.memory.position = meshBuffPointer + index * sizeOfInt * vSize;
			Library.memory.writeFloat(value);
			
			Library.memory.position = vDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		public function setVerticesY(index:int, value:Number):void
		{
			vertices[index].y = value;
			Library.memory.position = meshBuffPointer + index * sizeOfInt * vSize + sizeOfInt;
			Library.memory.writeFloat(value);
			
			Library.memory.position = vDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		public function setVerticesZ(index:int, value:Number):void
		{
			vertices[index].z = value;
			//获得顶点缓冲区的起始指针
			Library.memory.position = meshBuffPointer + index * sizeOfInt * vSize + sizeOfInt * 2;
			//写入数据
			Library.memory.writeFloat(value);
			
			Library.memory.position = vDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}
		
		public function fillVerticesToBuffer():void
		{
			Library.memory.position = meshBuffPointer;
			
			var v:Vertex3D;
			
			for each (v in vertices)
			{
				v.pointer = Library.memory.position + 4 * sizeOfInt;	//4代表保存指针的偏移量
				
				Library.memory.writeFloat(v.x);
				Library.memory.writeFloat(v.y);
				Library.memory.writeFloat(v.z);
				Library.memory.writeFloat(v.w);
				Library.memory.writeUnsignedInt(0);	//for saving pointer
			}
		}
		
		public function fillFacesToBuffer():void
		{
			Library.memory.position = meshBuffPointer + vertices.length * vSize * sizeOfInt;
			
			var f:Triangle3D;
			
			for each (f in faces)
			{
				Library.memory.writeUnsignedInt(f.v0.pointer);
				Library.memory.writeUnsignedInt(f.v1.pointer);
				Library.memory.writeUnsignedInt(f.v2.pointer);
				Library.memory.writeFloat(f.uv0.x);
				Library.memory.writeFloat(f.uv0.y);
				Library.memory.writeFloat(f.uv1.x);
				Library.memory.writeFloat(f.uv1.y);
				Library.memory.writeFloat(f.uv2.x);
				Library.memory.writeFloat(f.uv2.y);
				Library.memory.writeUnsignedInt(0);
				
				if (_material)
					Library.memory.writeUnsignedInt(_material.pointer);
				else
					Library.memory.writeUnsignedInt(0);
				
				if (_texture)
					Library.memory.writeUnsignedInt(_texture.pointer);
				else
					Library.memory.writeUnsignedInt(0);
			}
		}
		
		protected function applyForMeshBuffer():void
		{
			meshBuffPointer = Library.alchemy3DLib.applyForTmpBuffer((vertices.length * vSize + faces.length * fSize) * sizeOfInt);
		}
		
		override protected function initialize():void
		{
			fillVerticesToBuffer();
			fillFacesToBuffer();
			
			super.initialize();
		}
		
		override protected function callAlchemy():Array
		{
			var tPtr:uint = texture == null ? 0 : texture.pointer;
			var mPtr:uint = material == null ? 0 : material.pointer;
			
			return Library.alchemy3DLib.initializeEntity(mPtr, tPtr, name, meshBuffPointer, vertices.length, faces.length);
		}
		
		override protected function allotPtr(ps:Array):void
		{
			super.allotPtr(ps);
			
			lightEnablePtr		= ps[5];
			vDirtyPointer		= ps[6];
			fDirtyPointer		= ps[7];
		}
		
		protected function onTextureReady(e:LoadEvent):void
		{
			texture.removeEventListener(LoadEvent.TEXTURE_READY, onTextureReady);
			
			Library.alchemy3DLib.onTextureReady(this.pointer, texture.pointer);
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