package cn.alchemy3d.geom
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	public class Mesh extends Pointer
	{
		private var _vertices:uint;
		private var _faces   :uint;
		
		private var verticesPointer:uint;
		private var facesPointer   :uint;
		
		public function set vertices( vs:Vector.<Vertex> )
		{
			setVertices( vs );
			
			Library.instance().buffer.position = verticesPointer;
			
			Library.instance().buffer.writeUnsignedInt( _vertices );
		}
		
		public function set faces( fs:Vector.<Polygon> )
		{
			setFaces( fs );
			
			Library.instance().buffer.position = facesPointer;
			
			Library.instance().buffer.writeUnsignedInt( _faces );
		}
		
		public function Mesh( vertices:Vector.<Vertex>, faces:Vector.<Polygon> )
		{
			if( !vertices )
			{
				throw new Error( "No vertices!" );
			}
			
			init();
			
			setVertices( vertices );
			setFaces( faces );
			
			super();
		}
		
		private function init():void
		{
			_vertices = NULL;
			_faces    = NULL;
		}
		
		override private function initialize():void
		{
			var mesh:Array = Library.instance().methods.initializeMesh( _vertices, _faces );
			
			_pointer = mesh[0];
			
			verticesPointer = mesh[1];
			facesPointer    = mesh[2];
		}
		
		private function setVertices( vs:Vector.<Vertex> ):void
		{
			var v     :Vertex; 
			var vertex:Array;
			
			var vl    :uint = vs.length;
			var vp    :uint = Library.instance().methods.applyForTmpBuffer( vl * 3 * Library.instance().doubleTypeSize );
			 
			if( _vertices )
			{
				Library.instance().methods.destoryVertices( _vertices );
			}

			Library.instance().buffer.position = vp;
			
			for each ( v in vs )
			{
				Library.instance().buffer.writeDouble( v.x );
				Library.instance().buffer.writeDouble( v.y );
				Library.instance().buffer.writeDouble( v.z );
			}
			
			var pointer:Array        = Library.instance().methods.initializeVertices( vp, vl );
			
			_vertices                = pointer[0];
			
			Library.instance().buffer.position = pointer[1];

			for each( v in vs )
			{
				v.setPointer( Library.instance().buffer.readUnsignedInt() );
			}
		}
		
		private function setFaces( fs:Vector.<Polygon> ):void
		{
			var f     :Polygon;
			var i     :uint;
			var vertex:Vertex;
			var uv    :UV;
			
			var vl    :uint = 0;
			
			var fl    :uint = fs.length;
			
			var vp    :uint = Library.instance().methods.applyForTmpBuffer( fl     * Library.instance().intTypeSize()    );
			var uvp   :uint = Library.instance().methods.applyForTmpBuffer( fl * 2 * Library.instance().doubleTypeSize() );
			var lp    :uint = Library.instance().methods.applyForTmpBuffer( fl     * Library.instance().intTypeSize()    );
			
			if( _faces )
			{
				Library.instance().methods.destroyFaces( _faces );
			}
			
			Library.instance().buffer.position = vp;
			
			for each ( f in faces )
			{
				for( vertex in f.vertices )
				{
					if( !vertex.pointer )
					{
						throw new Error( "Do not match vertices!" );
					}
					
					Library.instance().buffer.writeUnsignedInt( vertex.pointer );
					
					vl ++;
				}
			}
			
			Library.instance().buffer.position = uvp;
			
			for each ( f in faces )
			{
				fl = f.vertices.length;
				
				for each( uv in f.uvs )
				{
					Library.instance().buffer.writeDouble( uv.u );
					Library.instance().buffer.writeDouble( uv.v );
				}
			}
			
			Library.instance().buffer.position = lp;
			
			for each ( f in faces )
			{
				Library.instance().buffer.writeUnsignedInt( f.vertices.length );
			}
			
			var pointer:Array = Library.instance().methods.initializeFaces( vp, uvp, lp, fl, vl );
			
			faces = pointer[0];
			
			Library.instance().buffer.position = pointer[1];
			
			for each ( f in faces )
			{
				f.setPointer( Library.instance().buffer.readUnsignedInt() );
			}
			
			Library.instance().buffer.position = pointer[2];
			
			for each ( f in faces )
			{
				fl = f.vertices.length;

				for each( uv in f.uvs )
				{
					uv.setPointer( Library.instance().buffer.readUnsignedInt() );
				}
			}
		}
	}
}