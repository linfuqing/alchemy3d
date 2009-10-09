package cn.alchemy3d.geom
{

	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	public class Mesh3D extends Pointer
	{
		//protected static const sizeOfInt:int = 4;
		private var vertices:Vector.<Vertex3D>;
		private var faces:Vector.<Triangle3D>;
		
		public function Mesh3D( vertices:Vector.<Vertex3D> = null, faces:Vector.<Triangle3D> = null )
		{	
			this._lightEnable = false;
			
			this.vertices = vertices;
			this.faces    =    faces;
			
			initialize();
		}
		
		public function get numVertices():uint
		{
			return vertices ? vertices.length : 0;
		}
		
		public function get numFaces():uint
		{
			return faces ? faces.length : 0;
		}
		
		private var lightEnablePointer:uint;
		private var fogEnablePointer:uint;
		private var useMipmapPointer:uint;
		private var mipDistPointer:uint;
		private var vDirtyPointer:uint;
		private var octreeDepthPointer:uint;
		
		private var _lightEnable:Boolean;
		
		private function setVertices( vs:Vector.<Vertex3D> ):uint
		{
			if( !vs )
			{
				return NULL;
			}
			
			var v     :Vertex3D; 
			
			var vl    :uint = vs.length;
			var vp    :uint = Library.alchemy3DLib.applyForTmpBuffer( vl * 3 * Library.floatTypeSize );

			Library.memory.position = vp;
			
			for each ( v in vs )
			{
				Library.memory.writeFloat( v.x );
				Library.memory.writeFloat( v.y );
				Library.memory.writeFloat( v.z );
			}
			
			return vp;
		}
		
		private function setFaces( fs:Vector.<Triangle3D> ):Vector.<uint>
		{
			if( !fs )
			{
				return new Vector.<uint>(5);
			}
			
			var index:Vector.<int> = new Vector.<int>(3);

			var fl    :uint = fs.length,
			
			    fp    :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize       * 3 ),
			    uvp   :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.floatTypeSize * 2 * 3 ),
			    mp    :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize           ),
			    tp    :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize           ),
			    rmp   :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize           );
			
			Library.memory.position = fp;
			
			for each ( var f:Triangle3D in fs )
			{
				if( ( ( index[0] = vertices.indexOf( f.v0 ) ) == - 1 )
				 || ( ( index[1] = vertices.indexOf( f.v1 ) ) == - 1 )
				 || ( ( index[2] = vertices.indexOf( f.v2 ) ) == - 1 ) )
				{
					throw new Error( "Do not match vertices!" );
				}
				
				for each( var i:uint in index )
				{
					Library.memory.writeUnsignedInt( index[i] );
				}
			}
			
			Library.memory.position = uvp;
			
			for each( f in fs )
			{
				Library.memory.writeFloat( f.uv0.x );
				Library.memory.writeFloat( f.uv0.y );
				Library.memory.writeFloat( f.uv1.x );
				Library.memory.writeFloat( f.uv1.y );
				Library.memory.writeFloat( f.uv2.x );
				Library.memory.writeFloat( f.uv2.y );
			}
			
			Library.memory.position = mp;
			
			for each( f in fs )
			{
				Library.memory.writeUnsignedInt( f._material.pointer );
			}
			
			Library.memory.position = tp;
			
			for each( f in fs )
			{
				Library.memory.writeUnsignedInt( f._texture ? f._texture.pointer : 0 );
			}
			
			Library.memory.position = rmp;
			
			for each( f in fs )
			{
				Library.memory.writeInt( f.renderMode );
			}
			
			return Vector.<uint>( [fp, uvp, mp, tp, rmp] );
		}
		
		/*public function get material():Material
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
		}*/
		
		/*
		public function set renderMode(mode:uint):void
		{
			for (var i:int = 0; i < faces.length; i ++)
			{
				Library.memory.position = meshBuffPointer + (vSize * vertices.length + 9 + i * fSize) * sizeOfInt;
				Library.memory.writeUnsignedInt(mode);
			}
			
			Library.memory.position = fDirtyPointer;
			Library.memory.writeUnsignedInt(1);
		}*/
		
		public function get useMipmap():Boolean
		{
			Library.memory.position = useMipmapPointer;
			return Library.memory.readBoolean();
		}
		
		public function set useMipmap(bool:Boolean):void
		{
			Library.memory.position = useMipmapPointer;
			Library.memory.writeBoolean(bool);
		}
		
		public function get mipDist():Number
		{
			Library.memory.position = mipDistPointer;
			return Library.memory.readFloat();
		}
		
		public function set mipDist(value:Number):void
		{
			Library.memory.position = mipDistPointer;
			Library.memory.writeFloat(value);
		}
		
		public function set octreeDepth(value:uint):void
		{
			Library.memory.position = octreeDepthPointer;
			Library.memory.writeInt(value);
		}
		
		public function get fogEnable():Number
		{
			Library.memory.position = fogEnablePointer;
			return Library.memory.readFloat();
		}
		
		public function set fogEnable(value:Number):void
		{
			Library.memory.position = fogEnablePointer;
			Library.memory.writeFloat(value);
		}
		
		public function get lightEnable():Boolean
		{
			return _lightEnable;
		}
		
		public function set lightEnable(bool:Boolean):void
		{
			_lightEnable = bool;
			
			Library.memory.position = lightEnablePointer;
			Library.memory.writeBoolean(bool);
		}
		
		/*public function setVertices(index:int, v:Vector3D):void
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
		}*/
		
		override protected function initialize():void
		{
			var p  :Vector.<uint> = setFaces( faces );
			var vp :uint          = setVertices( vertices ),
			
			    fp :uint          = p[0],
			    uvp:uint          = p[1],
			    mp :uint          = p[2],
			    tp :uint          = p[3],
			    rmp:uint          = p[4];
			    
			
			var ps:Array = Library.alchemy3DLib.initializeMesh( vp, fp, uvp, mp, tp, rmp,  numVertices, numFaces );
			
			_pointer            = ps[0];
			lightEnablePointer	= ps[1];
			fogEnablePointer	= ps[2];
			useMipmapPointer	= ps[3];
			mipDistPointer		= ps[4];
			vDirtyPointer		= ps[5];
			octreeDepthPointer	= ps[6];

			if( ps[7] && vertices )
			{
				Library.memory.position = ps[7];
			
				for( var i:uint = 0; i < vertices.length; i ++ )
				{
					vertices[i].setPointer( Library.memory.readUnsignedInt() );
				}
				
				Library.alchemy3DLib.freeTmpBuffer( ps[7] );
			}
		}
	}
}